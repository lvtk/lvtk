// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/widget.hpp>
#include <sol/sol.hpp>

namespace lvtk {
// proxies are needed to simulate class inherritence
// going from c++ to lua. lua to c++ inheritence not
// supported.... although it probably is possible.
namespace proxy {

static constexpr const char* impl_key    = "__impl";
static constexpr const char* base_key    = "__base";
static constexpr const char* atts_key    = "__atts";
static constexpr const char* props_key   = "__props";
static constexpr const char* methods_key = "__methods";

template <typename Obj>
static inline Obj* userdata (const sol::table& ctx) {
    if (! ctx.valid())
        return nullptr;
    auto mt     = ctx[sol::metatable_key];
    Obj* result = nullptr;

    try {
        if (mt[impl_key].get_type() == sol::type::userdata) {
            sol::object ud = mt[impl_key];
            result         = ud.as<Obj*>();
        }
    } catch (const std::exception&) {
    }

    return result;
};

template <typename Wgt>
static inline void widget_set_bounds (Wgt& self, const sol::object& obj) {
    if (obj.is<Bounds>()) {
        self.set_bounds (obj.as<Bounds>());
    } else if (obj.is<sol::table>()) {
        sol::table tr = obj;
        auto b        = self.bounds();
        self.set_bounds (
            tr.get_or ("x", b.x),
            tr.get_or ("y", b.y),
            tr.get_or ("width", b.width),
            tr.get_or ("height", b.height));
    }
}

// does nothing yet.
class Object {
public:
    Object()  = default;
    ~Object() = default;
};

//==
class Widget : public lvtk::Widget,
               public proxy::Object {
public:
    ~Widget();
    Widget (const sol::table& obj);

    static inline void init (const sol::table& ctx) {
        if (auto* const impl = proxy::userdata<proxy::Widget> (ctx)) {
            impl->widget = ctx;
        }
    }

    void resized() override;
    bool obstructed (int x, int y) override;
    void paint (Graphics& g) override;
    void motion (const Event& ev) override;
    void pressed (const Event& ev) override;
    void released (const Event& ev) override;
    sol::table add_with_z (const sol::object& child, int zorder);
    sol::table add (const sol::object& child);
    sol::table bounds_table();

    lvtk::Bounds proxy_get_bounds() const {
        return lvtk::Widget::bounds();
    }

    void proxy_set_bounds (sol::object obj) {
        auto r = bounds();
        if (obj.is<sol::table>()) {
            sol::table tbl = obj;
            set_bounds (
                tbl.get_or ("x", r.x),
                tbl.get_or ("y", r.y),
                tbl.get_or ("width", r.width),
                tbl.get_or ("height", r.height));
        } else if (obj.is<Bounds>()) {
            set_bounds (obj.as<Bounds>());
        }
    }

private:
    Widget() = delete;
    sol::table widget;
};

Widget::~Widget() {
    widget = sol::lua_nil;
}

Widget::Widget (const sol::table& obj) {
    widget = obj;
}

// TODO: cache the functions in a struct (or something).
void Widget::resized() {
    if (sol::safe_function f = widget["resized"])
        f (widget);
}

bool Widget::obstructed (int x, int y) {
    if (sol::safe_function f = widget["obstructed"]) {
        return (bool) f (widget, x, y);
    }
    return lvtk::Widget::obstructed (x, y);
}

void Widget::paint (Graphics& g) {
    if (sol::safe_function f = widget["paint"]) {
        f (widget, g);
    }
}

void Widget::motion (const Event& ev) {
    if (sol::safe_function f = widget["motion"])
        f (widget, ev);
}

void Widget::pressed (const Event& ev) {
    if (sol::safe_function f = widget["pressed"])
        f (widget, ev);
}

void Widget::released (const Event& ev) {
    if (sol::safe_function f = widget["released"])
        f (widget, ev);
}

sol::table Widget::add_with_z (const sol::object& child, int zorder) {
    (void) zorder; /// not used yet
    if (auto* const impl = proxy::userdata<proxy::Widget> (child))
        lvtk::Widget::add (*impl);

    return child;
}

sol::table Widget::add (const sol::object& child) {
    return add_with_z (child, -1);
}

sol::table Widget::bounds_table() {
    sol::state_view L (widget.lua_state());
    auto r      = bounds();
    auto t      = L.create_table();
    t["x"]      = r.x;
    t["y"]      = r.y;
    t["width"]  = r.width;
    t["height"] = r.height;
    return t;
}

} // namespace proxy
} // namespace lvtk
