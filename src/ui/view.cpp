// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <algorithm>

#include "lvtk/ui/input.hpp"
#include "lvtk/ui/main.hpp"
#include "lvtk/ui/view.hpp"
#include "lvtk/ui/widget.hpp"

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

// flip these for verbose logging
#include <iostream>
// #define VIEW_DBG(x) std::clog << x << std::endl;
#define VIEW_DBG(x)

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

inline static void erase (std::vector<View*>& views, View* view) {
    auto it = std::find (views.begin(), views.end(), view);
    if (it != views.end())
        views.erase (it);
}

} // namespace detail

struct View::EventHandler {
    static PuglStatus configure (View& view, const PuglConfigureEvent& ev) {
        if (ev.flags & PUGL_IS_HINT) {
            VIEW_DBG ("configure: hint");
            return PUGL_SUCCESS;
        }

        auto& w = view._widget;
        w.set_bounds (w.x(), w.y(), ev.width / view.scale_factor(), ev.height / view.scale_factor());
        VIEW_DBG ("pugl: configure: " << w.bounds().str());
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

        // std::clog << "expose: " << r.str() << std::endl;
        view.expose (r.intersection (view.bounds().at (0)));
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
        WidgetRef ref = view._widget.widget_at (pos);

        InputEvent event;

        if (ref.valid()) {
            event.pos = ref->convert (&view._widget, pos);
            if (view._hovered != ref) {
                VIEW_DBG ("hovered changed: " << ref->__name);
                ref->pointer_in (event);
                view._hovered = ref;
            }

            ref->motion (event);
        } else if (view._hovered) {
            VIEW_DBG ("hovered cleared");

            if (auto h = view._hovered.lock()) {
                event.pos   = h->convert (&view._widget, pos);
                event.pos.x = std::min (std::max (0.f, event.pos.x), (float) h->width());
                event.pos.y = std::min (std::max (0.f, event.pos.y), (float) h->height());
                h->pointer_out (event);
            }
            view._hovered = nullptr;
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus button_press (View& view, const PuglButtonEvent& ev) {
        InputEvent event;
        event.pos = detail::point<float> (ev) / view.scale_factor();

        VIEW_DBG ("button: " << (int) ev.button);

        if (auto w = view._hovered.lock()) {
            event.pos = w->convert (&view._widget, event.pos);
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
        auto hovered = view._hovered;

        if (auto w = hovered.lock()) {
            event.pos = hovered->convert (&view._widget, event.pos);
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

View::View (Main& m, Widget& w)
    : _main (m),
      _widget (w) {
    _view  = (uintptr_t) puglNewView ((PuglWorld*) m.world());
    auto v = (PuglView*) _view;
    puglSetSizeHint (v, PUGL_DEFAULT_SIZE, 10, 10);
    puglSetHandle (v, this);
    puglSetEventFunc (v, EventHandler::dispatch);
    _weak_status.reset (this);
    _main._views.push_back (this);
}

View::~View() {
    detail::erase (_main._views, this);
    _weak_status.reset();
    puglFreeView ((PuglView*) _view);
    _view = (uintptr_t) nullptr;
}

void View::set_backend (uintptr_t b) {
    puglSetBackend ((PuglView*) _view, (PuglBackend*) b);
}

void View::set_view_hint (int hint, int value) {
    puglSetViewHint ((PuglView*) _view, static_cast<PuglViewHint> (hint), value);
}

uintptr_t View::handle() { return puglGetNativeView ((PuglView*) _view); }
float View::scale_factor() const noexcept { return static_cast<float> (puglGetScaleFactor ((PuglView*) _view)); }

void View::set_visible (bool visible) {
    if (visible)
        puglShow ((PuglView*) _view);
    else
        puglHide ((PuglView*) _view);
}

bool View::visible() const { return puglGetVisible ((PuglView*) _view); }

void View::set_size (int width, int height) {
    puglSetSize ((PuglView*) _view, width * scale_factor(), height * scale_factor());
}

Rectangle<int> View::bounds() const {
    auto f = puglGetFrame ((PuglView*) _view);
    return {
        static_cast<int> ((float) f.x / scale_factor()),
        static_cast<int> ((float) f.y / scale_factor()),
        static_cast<int> ((float) f.width / scale_factor()),
        static_cast<int> ((float) f.height / scale_factor())
    };
}

void View::set_bounds (Bounds b) {
    b *= scale_factor();
    puglSetFrame ((PuglView*) _view, detail::frame (b));
}

void View::realize() {
    auto status = puglRealize ((PuglView*) _view);
    (void) status;
}

Style& View::style() noexcept { return _main.style(); }
const Style& View::style() const noexcept { return _main.style(); }

void View::elevate (Widget& widget, ViewFlags flags) {
    _main.elevate (widget, flags, *this);
}

void View::repaint (Bounds area) {
#if ! LVTK_WIDGET_USE_CLIPPING
    area = {};
#endif
    if (area.empty()) {
        puglPostRedisplay ((PuglView*) _view);
    } else {
        area *= scale_factor();
        puglPostRedisplayRect ((PuglView*) _view, { (PuglCoord) area.x, (PuglCoord) area.y, (PuglSpan) area.width, (PuglSpan) area.height });
    }
}

//==
void View::render (DrawingContext& ctx) {
    Graphics g (ctx);
    _widget.render (g);
}

void View::set_parent (uintptr_t parent, bool transient) {
    if (! parent)
        return;
    if (transient)
        puglSetTransientParent ((PuglView*) _view, parent);
    else
        puglSetParentWindow ((PuglView*) _view, parent);
}

} // namespace lvtk
