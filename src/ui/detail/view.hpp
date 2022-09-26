
#pragma once

#include <algorithm>

#include "lvtk/ui/input.hpp"
#include "lvtk/ui/main.hpp"
#include "lvtk/ui/view.hpp"
#include "lvtk/ui/widget.hpp"

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

// flip these for verbose logging at different levels
#include <iostream>
// #define VIEW_DBG(x) std::clog << "[view] " << x << std::endl;
#define VIEW_DBG(x)

// #define VIEW_DBG2(x) std::clog << "[view] " << x << std::endl;
#define VIEW_DBG2(x)

namespace lvtk {
namespace detail {
template <typename Tp, class Ev>
static inline Point<Tp> point (const Ev& ev) {
    return {
        static_cast<Tp> (ev.x),
        static_cast<Tp> (ev.y)
    };
}

template <typename Tp, class Ev>
static inline Rectangle<Tp> rect (const Ev& ev) {
    return {
        static_cast<Tp> (ev.x),
        static_cast<Tp> (ev.y),
        static_cast<Tp> (ev.width),
        static_cast<Tp> (ev.height)
    };
}

template <typename Rect>
static inline PuglRect frame (const Rect& r) {
    return {
        static_cast<PuglCoord> (r.x),
        static_cast<PuglCoord> (r.y),
        static_cast<PuglSpan> (r.width),
        static_cast<PuglSpan> (r.height)
    };
}

template <class Obj>
inline static void erase (std::vector<Obj*>& views, Obj* view) {
    auto it = std::find (views.begin(), views.end(), view);
    if (it != views.end())
        views.erase (it);
}

// The actual View impl. 
class View {
public:
    static constexpr uint32_t default_timer = 1000;
    View (lvtk::View& o, lvtk::Main& m, lvtk::Widget& w) 
        : owner (o), main(m), widget(w) {
        view  = puglNewView ((PuglWorld*) m.world());
        puglSetSizeHint (view, PUGL_DEFAULT_SIZE, 1, 1);
        puglSetHandle (view, this);
        puglSetEventFunc (view, dispatch);
    }

    ~View() {
        puglStopTimer (view, 0);
        puglFreeView (view);
        view = nullptr;
    }

    void set_backend (uintptr_t b) {
         puglSetBackend (view, (PuglBackend*) b);
    }

    void set_view_hint (int k, int v) {
        puglSetViewHint (view, static_cast<PuglViewHint> (k), v);
    }

    void set_parent (uintptr_t parent, bool transient) {
        if (transient)
            puglSetTransientParent (view, parent);
        else
            puglSetParentWindow (view, (PuglNativeView)parent);
    }

    float scale_factor() const noexcept {
        return puglGetScaleFactor (view);
    }
    
    void realize() {
        auto status = puglRealize (view);
        (void) status;
    }

private:
    friend class lvtk::View;
    friend class lvtk::Main;

    lvtk::View& owner;
    lvtk::Main& main;
    lvtk::Widget& widget;
    PuglView* view {nullptr};
    WeakRef<lvtk::Widget> hovered;

    static PuglStatus configure (View& view, const PuglConfigureEvent& ev) {
        if (ev.flags & PUGL_IS_HINT) {
            VIEW_DBG ("configure: hint");
            return PUGL_SUCCESS;
        }

        view.widget.set_bounds (view.widget.x(), 
                                view.widget.y(), 
                                ev.width / view.scale_factor(), 
                                ev.height / view.scale_factor());
        VIEW_DBG2 ("pugl: configure: " << view.widget.bounds().str());
        return PUGL_SUCCESS;
    }

    static PuglStatus expose (View& view, const PuglExposeEvent& ev) {
        if (ev.flags & PUGL_IS_HINT) {
            VIEW_DBG ("expose: hint");
            return PUGL_SUCCESS;
        }

        auto x = (float) ev.x / view.scale_factor();
        auto y = (float) ev.y / view.scale_factor();
        auto w = (float) ev.width / view.scale_factor();
        auto h = (float) ev.height / view.scale_factor();
        auto r = Rectangle<float> { x, y, w, h }.as<int>();

        VIEW_DBG2("expose: " << r.str());
        view.owner.expose (r.intersection (view.owner.bounds().at (0)));
        return PUGL_SUCCESS;
    }

    static PuglStatus create (View& view, const PuglCreateEvent& ev) {
        view.owner.created();
        puglStartTimer (view.view, 0, 14.0 / 1000.0);
        return PUGL_SUCCESS;
    }

    static PuglStatus destroy (View& view, const PuglDestroyEvent& ev) {
        puglStopTimer (view.view, 0);
        view.owner.destroyed();
        return PUGL_SUCCESS;
    }

    static PuglStatus map (View& view, const PuglMapEvent& ev) {
        return PUGL_SUCCESS;
    }

    static PuglStatus unmap (View& view, const PuglUnmapEvent& ev) {
        return PUGL_SUCCESS;
    }

    static void update_recursive (Widget& widget) {
        if (widget.__wants_updates())
            widget.__update();
        for (auto& child : widget.__widgets())
            update_recursive (*child);
    }

    static void idle_recursive (Widget& widget) {
        if (widget.__wants_updates())
            widget.__update();
        for (auto& child : widget.__widgets())
            update_recursive (*child);
    }

    static PuglStatus update (View& view, const PuglUpdateEvent& ev) {
        update_recursive (view.widget);
        return PUGL_SUCCESS;
    }

    static PuglStatus close (View& view, const PuglCloseEvent& ev) {
        // dirty setup so the demo can at least quit...
        if (view.main.mode() == Mode::PROGRAM)
            view.main.quit();
        return PUGL_SUCCESS;
    }

    static PuglStatus focus_in (View& view, const PuglFocusEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus focus_out (View& view, const PuglFocusEvent& ev) { return PUGL_SUCCESS; }

    static PuglStatus key_press (View& view, const PuglKeyEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus key_release (View& view, const PuglKeyEvent& ev) { return PUGL_SUCCESS; }

    static PuglStatus text (View& view, const PuglTextEvent& ev) { return PUGL_SUCCESS; }

    static PuglStatus pointer_in (View& view, const PuglCrossingEvent& ev) {
        VIEW_DBG ("pointer in")
        return PUGL_SUCCESS;
    }

    static PuglStatus pointer_out (View& view, const PuglCrossingEvent& ev) {
        VIEW_DBG ("pointer out")
        return PUGL_SUCCESS;
    }

    static PuglStatus motion (View& view, const PuglMotionEvent& ev) {
        auto pos      = detail::point<float> (ev) / view.scale_factor();
        WidgetRef ref = view.widget.widget_at (pos);

        InputEvent event;

        if (ref.valid()) {
            event.pos = ref->convert (&view.widget, pos);
            if (view.hovered != ref) {
                VIEW_DBG ("hovered changed: " << ref->__name);
                ref->pointer_in (event);
                view.hovered = ref;
            }

            ref->motion (event);
        } else if (view.hovered) {
            VIEW_DBG ("hovered cleared");

            if (auto h = view.hovered.lock()) {
                event.pos   = h->convert (&view.widget, pos);
                event.pos.x = std::min (std::max (0.f, event.pos.x), (float) h->width());
                event.pos.y = std::min (std::max (0.f, event.pos.y), (float) h->height());
                h->pointer_out (event);
            }
            view.hovered = nullptr;
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus button_press (View& view, const PuglButtonEvent& ev) {
        InputEvent event;
        event.pos = detail::point<float> (ev) / view.scale_factor();

        VIEW_DBG ("button: " << (int) ev.button);

        if (auto w = view.hovered.lock()) {
            event.pos = w->convert (&view.widget, event.pos);
            // VIEW_DBG("widget:" << w->__name << " bounds: " << w->bounds().str());
            // VIEW_DBG("ev pos: " << event.pos.str());
            if (w->contains (event.pos))
                w->pressed (event);
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus button_release (View& view, const PuglButtonEvent& ev) {
        InputEvent event;
        event.pos    = detail::point<float> (ev) / view.scale_factor();
        auto hovered = view.hovered;

        if (auto w = hovered.lock()) {
            event.pos = hovered->convert (&view.widget, event.pos);
            if (w->contains (event.pos))
                w->released (event);
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus scroll (View& view, const PuglScrollEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus client (View& view, const PuglClientEvent& ev) { return PUGL_SUCCESS; }

    static PuglStatus timer (View& view, const PuglTimerEvent& ev) {
        idle_recursive (view.widget);
        return PUGL_SUCCESS;
    }

    static PuglStatus loop_enter (View& view, const PuglLoopEnterEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus loop_leave (View& view, const PuglLoopLeaveEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus data_offer (View& view, const PuglDataOfferEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus data (View& view, const PuglDataEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus nothing (View&, const PuglAnyEvent&) { return PUGL_SUCCESS; }

    static inline PuglStatus dispatch (PuglView* v, const PuglEvent* ev) {
        auto view         = static_cast<View*> (puglGetHandle (v));
        PuglStatus status = PUGL_SUCCESS;

#define CASE3(t, fn, arg)         \
    case t:                       \
        status = fn (*view, arg); \
        break;
#define CASE2(t, fn) CASE3 (t, fn, ev->fn)
#define CASEA(t, fn) CASE3 (t, fn, ev->any)
        switch (ev->type) {
            CASE2 (PUGL_CONFIGURE, configure)                       //< View moved/resized, a #PuglConfigureEvent
            CASEA (PUGL_UPDATE, update)                             //< View ready to draw, a #PuglUpdateEvent
            CASE2 (PUGL_EXPOSE, expose)                             //< View must be drawn, a #PuglExposeEvent
            CASEA (PUGL_CREATE, create)                             //< View created, a #PuglCreateEvent
            CASEA (PUGL_DESTROY, destroy)                           ///< View destroyed, a #PuglDestroyEvent
            CASEA (PUGL_MAP, map)                                   ///< View made visible, a #PuglMapEvent
            CASEA (PUGL_UNMAP, unmap)                               ///< View made invisible, a #PuglUnmapEvent
            CASEA (PUGL_CLOSE, close)                               ///< View will be closed, a #PuglCloseEvent
            CASE3 (PUGL_FOCUS_IN, focus_in, ev->focus)              ///< Keyboard focus entered view, a #PuglFocusEvent
            CASE3 (PUGL_FOCUS_OUT, focus_out, ev->focus)            ///< Keyboard focus left view, a #PuglFocusEvent
            CASE3 (PUGL_KEY_PRESS, key_press, ev->key)              ///< Key pressed, a #PuglKeyEvent
            CASE3 (PUGL_KEY_RELEASE, key_release, ev->key)          ///< Key released, a #PuglKeyEvent
            CASE2 (PUGL_TEXT, text)                                 ///< Character entered, a #PuglTextEvent
            CASE3 (PUGL_POINTER_IN, pointer_in, ev->crossing)       ///< Pointer entered view, a #PuglCrossingEvent
            CASE3 (PUGL_POINTER_OUT, pointer_out, ev->crossing)     ///< Pointer left view, a #PuglCrossingEvent
            CASE3 (PUGL_BUTTON_PRESS, button_press, ev->button)     ///< Mouse button pressed, a #PuglButtonEvent
            CASE3 (PUGL_BUTTON_RELEASE, button_release, ev->button) ///< Mouse button released, a #PuglButtonEvent
            CASE2 (PUGL_MOTION, motion)                             ///< Pointer moved, a #PuglMotionEvent
            CASE2 (PUGL_SCROLL, scroll)                             ///< Scrolled, a #PuglScrollEvent
            CASE2 (PUGL_CLIENT, client)                             ///< Custom client message, a #PuglClientEvent
            CASE2 (PUGL_TIMER, timer)                               ///< Timer triggered, a #PuglTimerEvent
            CASEA (PUGL_LOOP_ENTER, loop_enter)                     ///< Recursive loop entered, a #PuglLoopEnterEvent
            CASEA (PUGL_LOOP_LEAVE, loop_leave)                     ///< Recursive loop left, a #PuglLoopLeaveEvent
            CASE3 (PUGL_DATA_OFFER, data_offer, ev->offer)          ///< Data offered from clipboard, a #PuglDataOfferEvent
            CASE2 (PUGL_DATA, data)                                 ///< Data available from clipboard, a #PuglDataEvent
            CASE3 (PUGL_NOTHING, nothing, ev->any)                  ///< No event
            default:
                break;
        }
#undef CASE2
#undef CASE3
#undef CASEA
        return status;
    }
};

} // namespace detail
}
