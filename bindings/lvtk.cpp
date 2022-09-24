// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "proxy.hpp"
#include <lvtk/lvtk.hpp>
#include <lvtk/string.hpp>
#include <lvtk/ui/opengl.hpp>

#include "./lvtk/bindings.h"

namespace sol {
#if 0
/** Support lvtk::WeakRef in sol2. Does not work yet  */
template <typename T>
struct unique_usertype_traits<lvtk::WeakRef<T>>
{
    using type = lvtk::WeakRef<T>;
    using actual_type = T;
    static const bool value = true;
    static bool is_null (const actual_type& ptr) { return ptr == nullptr; }
    static type* get (const actual_type& ptr) { return ptr.lock(); }
};
#endif
} // namespace sol

using namespace sol;
using namespace lvtk;

namespace lua {

// Lua Binding Helpers
// This was thrown together, but could be tidy'd up and added
// to the public headers. Coders could then write custom widgets
// and have a helper library to bind them to Lua.  Class inheritence
// is working but there's alot of manual steps involved.

/** Removes a field from the table then clears it.
    
    @param tbl Input table
    @param field The field to remove. Lua type MUST be a table
*/
inline static table remove_and_clear (table tbl, const char* field) {
    auto F = tbl.get<table> (field);
    tbl.clear();
    return F;
}

template <class Self>
inline static std::string tostring (Self& self, const char* NS, const char* name) {
    std::stringstream stream;
    stream << NS << "." << name << ": 0x" << std::hex << (intptr_t) &self;
    return stream.str();
}

//=======
/* Class loading.  This is kind of a crude, hard-coded, class loader 
   This is not fool proof whatsoever */
static inline void require_core_types (lua_State* L) {
}

static inline void require_geometry_types (lua_State* L) {
}

// deps required by Widget, excluding itself
//
static inline void require_widget_deps (lua_State* L) {
    state_view view (L);
    view.script (R"(
        require ('lvtk.Point')
        require ('lvtk.Bounds')
        require ('lvtk.Graphics')
    )");
}

//====== Class binding helpers
/** Bind adding tostring meta method */
template <typename Obj, typename... Args>
inline static auto bind (lua_State* L, const char* NS, const char* name, Args&&... args) {
    state_view lua (L);
    table M = lua.create_table();
    M.new_usertype<Obj> (
        name, no_constructor, meta_method::to_string, [&] (Obj& self) {
            return lua::tostring (self, NS, name);
        },
        std::forward<Args> (args)...);
    return remove_and_clear (M, name);
}

/** binds without adding tostring meta method */
template <typename Obj, typename... Args>
inline static auto bind (lua_State* L, const char* name, Args&&... args) {
    state_view lua (L);
    table M = lua.create_table();
    M.new_usertype<Obj> (
        name, no_constructor, std::forward<Args> (args)...);
    return remove_and_clear (M, name);
}

inline static auto script (lua_State* L, const char* body) {
    return state_view (L).script (body);
}

/** Subclasses of Widget in C++ should register with this variation
 *  then call require_widget_deps before pushing to the stack
 */
template <typename Wgt, typename... Args>
inline static table
    bind_widget (lua_State* L, const char* name, Args&&... args) {
    // clang-format off
    auto M = lua::bind<Wgt> (L, "lvtk", name,
        /// Initialize the widget.
        // Override this to customize your widget.
        // @function Widget.init
        "init",
        Wgt::init,

        /// Widget visibility (bool).
        // Shows or hides this Widget
        // @field Widget.visible
        "visible",
        property (&Wgt::visible, &Wgt::set_visible),

        /// Methods.
        // @section methods

        /// Returns the bounding box.
        // @function Widget:bounds
        "bounds", property(&Wgt::proxy_get_bounds, &Wgt::proxy_set_bounds),
        // "bounds", &Wgt::bounds,

        "obstructed",
        &Wgt::obstructed,

        /// Change the bounding box.
        // The coords returned is relative to the top/left of the widget's parent.
        // @function Widget:setbounds
        // @tparam mixed New bounds as a kv.Bounds or table
        // @usage
        // -- Can also set a table. e.g.
        // widget:setbounds ({
        //     x      = 0,
        //     y      = 0,
        //     width  = 100,
        //     height = 200
        // })
        "set_bounds", 
        overload (
            [] (Wgt& self, double x, double y, double w, double h) { 
                self.set_bounds (lvtk::Rectangle<double> { x, y, w, h }.as<int>()); 
            }, [](Wgt& self, const object& obj) { 
                lvtk::proxy::widget_set_bounds (self, obj);
            }),
        "set_size", &Wgt::set_size,
        std::forward<Args> (args)...);
    // clang-format on
    state_view view (L);

    ////
    // begin C++ <=> Lua class inheritance setup
    ////

    // turn of "operator[] so attributes defined in lua
    // can be resolved
    auto M_mt          = M[metatable_key];
    M_mt["__newindex"] = lua_nil;

    // Called when widget is instantiated by lvtk.object
    M_mt["__newuserdata"] = [L]() {
        state_view view (L);
        return std::make_unique<Wgt> (view.create_table());
    };

    // attribute lookup keys used by lua-side proxy
    M_mt[lvtk::proxy::props_key] = view.create_table().add (
        "visible", "bounds");

    // class method symbols used by lua-side proxy
    M_mt[lvtk::proxy::methods_key] = view.create_table().add (
        "add",
        "set_bounds",
        "set_size");

    require_widget_deps (L);
    view.script (R"(
        require ('lvtk.Graphics')
    )");
    return M;
}

} // namespace lua

// clang-format off
//=============================================================================
#include <lvtk/context.hpp>
LVTK_LUALIB
int luaopen_lvtk_Context (lua_State* L) {
    // This'll need re-done in plain lua if it ever
    // becomes a performance hit in realtime/dsp
    // contexts.
    auto M = lua::bind<lvtk::Context> (L, "lvtk", "Context",
        "symbols", &Context::symbols
    );

    lua::script (L, R"(
        require ('lvtk.Graphics')
    )");
    
    stack::push (L, M);
    return 1;
}

//=============================================================================
#include <lvtk/symbols.hpp>
LVTK_LUALIB
int luaopen_lvtk_Symbols (lua_State* L) {
    auto M = lua::bind<lvtk::Symbols> (L, "lvtk", "Symbols",
        "map", [](Symbols& self, const char* uri) -> lua_Integer {
            return static_cast<lua_Integer> (self.map (uri));
        },
        "unmap", [](Symbols& self, lua_Integer urid) -> std::string {
            return self.unmap (static_cast<LV2_URID> (urid));
        }
    );
    lua::require_core_types (L);
    stack::push (L, M);
    return 1;
}

//=============================================================================
#include <lvtk/ui/graphics.hpp>
LVTK_LUALIB
int luaopen_lvtk_Point (lua_State* L) {
    using Point = Point<float>;
    using T = float;
    auto M = lua::bind<Point> (L, "lvtk", "Point",
        "x",        &Point::x,
        "y",        &Point::y,
        "new", factories (
            []() { return Point(); },
            [](T x, T y) { return Point{ x, y }; }
        ),
        meta_method::to_string, [](const Point& self) {
            auto s = lua::tostring (self, "lvtk", "Point");
            s += std::string(": ") + self.str();
            return s;
        }
    );

    // doesn't depend on anything, no requires
    stack::push (L, M);
    return 1;
}

LVTK_LUALIB
int luaopen_lvtk_Bounds (lua_State* L) {
    using R = Bounds;
    using T = int;
    auto M = lua::bind<Bounds> (L, "lvtk", "Bounds",
        "x",        &Bounds::x,
        "y",        &Bounds::y,
        "width",    &Bounds::width,
        "height",   &Bounds::height,
        "at",    [](Bounds& self, lua_Integer x, lua_Integer y) -> R {
            return self.at (x, y);
        },

        "new", factories (
            []() { return R(); },
            [](T x, T y, T w, T h) { return R{ x, y, w, h }; },
            [](T w, T h) { return R { T(), T(), w, h }; }
        ),

        meta_method::to_string, [](const Bounds& self) {
            auto s = lua::tostring (self, "lvtk", "Bounds");
            s += std::string(": ") + self.str();
            return s;
        }
    );

    state_view(L).script (R"(
        require ('lvtk.Point')
    )");
    stack::push (L, M);
    return 1;
}

LVTK_LUALIB
int luaopen_lvtk_Graphics (lua_State* L) {
    static constexpr const char* ns = "lvtk";
    static constexpr const char* name = "Graphics";
    auto M = lua::bind<lvtk::Graphics> (L, ns, name,
        "last_clip", &Graphics::last_clip,
        "translate", &Graphics::translate,
        "save", &Graphics::save,
        "restore", &Graphics::restore,
        "set_color", [](Graphics& self, lua_Integer c) {
            self.set_color (lvtk::Color (static_cast<uint32_t> (c)));
            self.set_color (lvtk::Color (0xff0000ff));
        },
        "fill_rect", [](Graphics& self, Bounds r) {
                self.fill_rect (r);
            }
    );

    lua::require_geometry_types (L);
    stack::push (L, M);
    return 1;
}

LVTK_LUALIB
int luaopen_lvtk_Surface (lua_State* L) {
    static constexpr const char* ns = "lvtk";
    static constexpr const char* name = "DrawingContext";
    auto M = lua::bind<lvtk::DrawingContext> (L, ns, name,
        "dummy", []() {}
    );

    lua::require_geometry_types (L);
    stack::push (L, M);
    return 1;
}

//=============================================================================
#include <lvtk/ui/input.hpp>
LVTK_LUALIB
int luaopen_lvtk_Event (lua_State* L) {
    auto T = lua::bind<lvtk::Event> (L, "lvtk", "Event",
        "source", readonly_property (&Event::source),
        "pos", readonly_property (&Event::pos),
        "x", readonly_property (&Event::x),
        "y", readonly_property (&Event::y)
    );

    stack::push (L, T);
    return 1;
}

//=============================================================================
#include <lvtk/ui/main.hpp>
LVTK_LUALIB
int luaopen_lvtk_Main (lua_State* L) {
    auto T = lua::bind<lvtk::Main> (L, "lvtk", "Main",
        "loop",     &Main::loop,
        "elevate", [](lvtk::Main& self, object tbl) {
            // std::clog << "elevate: " << type_name (L, tbl.get_type()) << std::endl;
            // TODO: deeply inspect that this really is a widget
            // if not rais a lua error
            if (tbl.is<table>()) {
                if (auto proxy = lvtk::proxy::userdata<lvtk::proxy::Widget> ((table)tbl))
                    self.elevate (*proxy, 0, 0);
            } else {
                // std::clog << "not a table: \n";
            }
        },
        
        "mode", [](lvtk::Main& self) -> std::string {
            switch (self.mode()) {
                case lvtk::Mode::PROGRAM: return "program"; break;
                case lvtk::Mode::MODULE: return "module"; break;
            };
            return "undefined";
        },

        "quit", &Main::quit,
        
        "__quit_flag", &Main::__quit_flag,

        "symbols",  &Main::symbols,
        "new", factories ([]() {
            auto obj = std::make_unique<lvtk::Main>(
                lvtk::Mode::PROGRAM,
                std::make_unique<lvtk::OpenGL>()
            );

            return obj;
        })
    );

    lua::script (L, R"(
        require ('lvtk.Graphics')
        require ('lvtk.Widget')
        require ('lvtk.View')
        require ('lvtk.Event')
    )");

    stack::push (L, T);
    return 1;
}

//=============================================================================
#include <lvtk/ui/view.hpp>
LVTK_LUALIB
int luaopen_lvtk_View (lua_State* L) {
    auto T = lua::bind<lvtk::View> (L, "lvtk", "View",
        "visible",  property (&View::visible, &View::set_visible),
        "bounds",       &View::bounds,
        "scale_factor", &View::scale_factor,
        "set_size",     &View::set_size
    );
    stack::push (L, T);
    return 1;
}

//=============================================================================
#include <lvtk/ui/widget.hpp>
LVTK_LUALIB
int luaopen_lvtk_Widget (lua_State* L) {
    using W = lvtk::proxy::Widget;
    auto M = lua::bind_widget<W> (L, "Widget", 
        "add",      &W::add
    );

    lua::require_widget_deps (L);
    stack::push (L, M);
    return 1;
}

#include <lvtk/host/world.hpp>
LVTK_LUALIB
int luaopen_lvtk_World (lua_State* L) {
    auto T = lua::bind<lvtk::World> (L, "lvtk", "World",
        "load_all", &lvtk::World::load_all,
        "instantiate", &lvtk::World::instantiate,
        "new", factories ([]() {
            return std::make_unique<lvtk::World>();
        })
    );

    stack::push (L, T);
    return 1;
}

#include <lvtk/host/instance.hpp>
// using sol probably isn't good enough for realtime. This might
// need re-written in vanilla lua.
int luaopen_lvtk_Instance (lua_State* L) {
    auto T = lua::bind<lvtk::Instance> (L, "lvtk", "Instance",
        "activate", [](Instance&) {},
        "deactivate", [](Instance&) {},
        "run", [](Instance&) {},
        "instantiate_ui", [](Instance&) {}
    );

    stack::push (L, T);
    return 1;
}

int luaopen_lvtk_InstanceUI (lua_State* L) {
    auto T = lua::bind<lvtk::InstanceUI> (L, "lvtk", "InstanceUI",
        "loaded", &lvtk::InstanceUI::loaded,
        "unload", &lvtk::InstanceUI::unload,
        "widget", [](lvtk::InstanceUI& self) -> lua_Integer {
            return (lua_Integer) self.widget();
        }
    );

    stack::push (L, T);
    return 1;
}

// main module
LVTK_LUALIB
int luaopen_lvtk (lua_State* L) {
    // lock and load
    state_view view (L);
    stack::push (L, view.script (R"(
    local M = {
        bytes       = require ('lvtk.bytes'),
        import      = require ('lvtk.import'),
        object      = require ('lvtk.object'),
        midi        = require ('lvtk.midi'),
        round       = require ('lvtk.round'),
        
        Point       = require ('lvtk.Point'),
        Bounds      = require ('lvtk.Bounds'),
        Surface     = require ('lvtk.Surface'),
        Graphics    = require ('lvtk.Graphics'),
        Event       = require ('lvtk.Event'),
        Main        = require ('lvtk.Main'),
        View        = require ('lvtk.View'),
        Widget      = require ('lvtk.Widget'),
        World       = require ('lvtk.World'),
        Symbols     = require ('lvtk.Symbols')
    }
    return M
    )"));

    return 1;
}

// clang-format on
