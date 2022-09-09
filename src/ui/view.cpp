// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "lvtk/ui/view.hpp"
#include "lvtk/ui/input.hpp"
#include "lvtk/ui/main.hpp"
#include "lvtk/ui/widget.hpp"

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

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
} // namespace detail

struct View::EventHandler {
    static PuglStatus configure (View& view, const PuglConfigureEvent& ev) {
        // view.configure (detail::rect<int> (ev));
        return PUGL_SUCCESS;
    }

    static PuglStatus expose (View& view, const PuglExposeEvent& ev) {
        view.expose (detail::rect<int> (ev));
        return PUGL_SUCCESS;
    }

    static PuglStatus create (View& view, const PuglCreateEvent& ev) {
        view.created();
        return PUGL_SUCCESS;
    }

    static PuglStatus destroy (View& view, const PuglDestroyEvent& ev) {
        view.destroyed();
        return PUGL_SUCCESS;
    }

    static PuglStatus map (View& view, const PuglMapEvent& ev) {
        return PUGL_SUCCESS;
    }

    static PuglStatus unmap (View& view, const PuglUnmapEvent& ev) {
        return PUGL_SUCCESS;
    }

    static PuglStatus update (View& view, const PuglUpdateEvent& ev) {
        return PUGL_SUCCESS;
    }

    static PuglStatus close (View& view, const PuglCloseEvent& ev) {
        // dirty setup so the demo can at least quit...
        if (view._main.mode() == Mode::PROGRAM)
            view._main.quit();
        return PUGL_SUCCESS;
    }

    static PuglStatus focus_in (View& view, const PuglFocusEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus focus_out (View& view, const PuglFocusEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus key_press (View& view, const PuglKeyEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus key_release (View& view, const PuglKeyEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus text (View& view, const PuglTextEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus pointer_in (View& view, const PuglCrossingEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus pointer_out (View& view, const PuglCrossingEvent& ev) { return PUGL_SUCCESS; }

    static PuglStatus motion (View& view, const PuglMotionEvent& ev) {
        auto pos = detail::point<double> (ev);
        WidgetRef ref = view._widget.widget_at (pos.as<float>());

        if (ref.valid()) {
            InputEvent event;

            if (view._hovered != ref) {
                std::clog << "hovered changed: " << ref->__name << std::endl;
                view._hovered = ref;
            }

            event.pos = ref->convert (view._widget, pos).as<double>();
            ref->motion (event);
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus button_press (View& view, const PuglButtonEvent& ev) {
        std::clog << "button_press received\n";
        InputEvent event;
        event.pos = detail::point<double> (ev);

        if (auto w = view._hovered.lock()) {
            event.pos = w->convert (view._widget, event.pos);
            std::clog << "widget:" << w->__name << " bounds: " << w->bounds().str() << "\n";
            std::clog << "ev pos: " << event.pos.str() << std::endl;
            if (w->contains (event.pos))
                w->pressed (event);
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus button_release (View& view, const PuglButtonEvent& ev) {
        InputEvent event;
        event.pos = detail::point<double> (ev);
        auto hovered = view._hovered;

        if (auto w = hovered.lock()) {
            event.pos = hovered->convert (view._widget, event.pos);
            if (w->contains (event.pos))
                w->released (event);
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus scroll (View& view, const PuglScrollEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus client (View& view, const PuglClientEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus timer (View& view, const PuglTimerEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus loop_enter (View& view, const PuglLoopEnterEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus loop_leave (View& view, const PuglLoopLeaveEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus data_offer (View& view, const PuglDataOfferEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus data (View& view, const PuglDataEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus nothing (View&, const PuglAnyEvent&) { return PUGL_SUCCESS; }

    static inline PuglStatus dispatch (PuglView* v, const PuglEvent* ev) {
        auto view = static_cast<View*> (puglGetHandle (v));
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

View::View (Main& m, Widget& w)
    : _main (m),
      _widget (w) {
    _view = (uintptr_t) puglNewView ((PuglWorld*) m.world());
    puglSetHandle ((PuglView*) _view, this);
    puglSetEventFunc ((PuglView*) _view, EventHandler::dispatch);
    _weak_status.reset (this);
}

View::~View() {
    _weak_status.reset();
    puglFreeView ((PuglView*) _view);
    _view = (uintptr_t) nullptr;
}

void View::set_backend (uintptr_t b) {
    puglSetBackend ((PuglView*) _view, (PuglBackend*) b);
}

uintptr_t View::handle() { return puglGetNativeView ((PuglView*) _view); }
double View::scale() const noexcept { return puglGetScaleFactor ((PuglView*) _view); }

void View::set_visible (bool visible) {
    if (visible)
        puglShow ((PuglView*) _view);
    else
        puglHide ((PuglView*) _view);
}

bool View::visible() const { return puglGetVisible ((PuglView*) _view); }

void View::set_size (int width, int height) {
    puglSetSize ((PuglView*) _view, width, height);
}

Rectangle<int> View::bounds() const {
    auto f = puglGetFrame ((PuglView*) _view);
    return {
        static_cast<int> (f.x),
        static_cast<int> (f.y),
        static_cast<int> (f.width),
        static_cast<int> (f.height)
    };
}

void View::realize() {
    puglRealize ((PuglView*) _view);
}

//==
void View::render (Surface& ctx) {
    Graphics g (ctx);
    _widget.render (g);
}

void View::set_parent (uintptr_t parent) {
    if (parent)
        puglSetParentWindow ((PuglView*) _view, parent);
}

} // namespace lvtk
