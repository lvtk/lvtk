// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ext/atom.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>

#include <lvtk/ui/input.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/view.hpp>
#include <lvtk/ui/widget.hpp>

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

#include "ui/detail/main.hpp"
#include "ui/detail/widget.hpp"

#include "ui/detail/main.hpp"

#define LVTK_MAX_BUTTONS 4

#define VIEW_DBG(x) std::clog << "[view] " << x << std::endl;
// #define VIEW_DBG(x)

#define VIEW_DBG2(x) std::clog << "[view] " << x << std::endl;
// #define VIEW_DBG2(x)

// #define VIEW_DBG3(x) std::clog << "[view] " << x << std::endl;
#define VIEW_DBG3(x)

// #define VIEW_DBG4(x) std::clog << "[view] " << x << std::endl;
#define VIEW_DBG4(x)

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

namespace input {

static Event event (lvtk::Main& m,
                    lvtk::Widget& src,
                    lvtk::Widget& tgt,
                    Modifier mods,
                    Point<float> pos,
                    int num_clicks) {
    return { m,
             tgt.convert (&src, pos),
             mods,
             &src,
             &tgt,
             num_clicks };
}

static Event event (lvtk::Main& m,
                    lvtk::Widget& src,
                    lvtk::Widget& tgt,
                    Point<float> pos) {
    return event (m, src, tgt, Modifier(), pos, 0);
}

} // namespace input

/** A key event */
struct KeyState {
    PuglKeyEvent event;
    bool down = false;
    lvtk::Widget* source { nullptr };
    lvtk::Widget* target { nullptr };
    KeyState& set (const PuglKeyEvent& pev) {
        event = pev;
        return *this;
    }
};

/** Keeps track of Pugl key press and release events. */
class Keyboard {
public:
    Keyboard() {
        reset();
    }

    void reset() {
        for (uint32_t key = 0; key < 256; ++key)
            clear_key (key);
        clear_key (PUGL_KEY_BACKSPACE);
        clear_key (PUGL_KEY_ESCAPE);
        clear_key (PUGL_KEY_DELETE);
        for (uint32_t key = PUGL_KEY_F1; key <= PUGL_KEY_PAUSE; ++key)
            clear_key (key);
    }

    void press (lvtk::View& view, const PuglKeyEvent& pev) {
        auto& ev  = down[pev.keycode].set (pev);
        ev.down   = true;
        ev.source = &view.widget();
        ev.target = nullptr;
    }

    void release (lvtk::View& view, const PuglKeyEvent& pev) {
        auto& ev  = up[pev.keycode].set (pev);
        ev.down   = false;
        auto& dev = down.at (pev.keycode);
        dev.down  = false;
    }

    lvtk::Widget* target (uint32_t key) const noexcept {
        return down.at (key).target;
    }

    void set_target (uint32_t key, lvtk::Widget* tgt) {
        down.at (key).target = tgt;
    }

private:
    std::unordered_map<uint32_t, KeyState> down;
    std::unordered_map<uint32_t, KeyState> up;

    void clear_key (uint32_t key, bool pressed) {
        auto& mp = pressed ? down : up;
        mp[key]  = {};
    }

    void clear_key (uint32_t key) {
        clear_key (key, true);
        clear_key (key, false);
    }
};

struct ButtonState {
    PuglButtonEvent event;
    ViewRef view;
    lvtk::Widget* source;
    lvtk::Widget* target;

    bool down = false;

    ButtonState() {}
    ButtonState (const ButtonState& o) { operator= (o); }
    ButtonState& operator= (const ButtonState& o) {
        event  = o.event;
        view   = o.view;
        source = o.source;
        target = o.target;
        down   = o.down;
        return *this;
    }

    ButtonState& set (const PuglButtonEvent& pev) {
        event = pev;
        return *this;
    }
};

/** Keeps track of Pugl button press and release events.
    Tallies a click count for each button as well as
    what it currently pressed and associated Widgets
 */
class Buttons {
public:
    Buttons() {
        reset();
    }

    /** Reset everything */
    void reset() {
        for (uint32_t i = 0; i < LVTK_MAX_BUTTONS; ++i)
            reset (i);
    }

    /** Reset individual button */
    void reset (uint32_t button) {
        clear_state (up[button]);
        clear_state (down[button]);
        click_count[button] = 0;
        mods                = mods.without_flags (button_to_flag (button));
    }

    /** Returns true if any button is down */
    bool is_down() const noexcept {
        for (int i = 0; i < LVTK_MAX_BUTTONS; ++i)
            if (down[i].down == true)
                return true;
        return false;
    }

    /** returns true if any button is down with widget as the target */
    bool is_down (lvtk::Widget& widget) const noexcept {
        for (int i = 0; i < LVTK_MAX_BUTTONS; ++i)
            if (down[i].down == true && down[i].target == &widget)
                return true;
        return false;
    }

    bool is_down (int button) const noexcept { return down[button].down; }

    uint32_t button_to_flag (uint32_t button) {
        switch (button) {
            case 0:
                return Modifier::LEFT_BUTTON;
            case 1:
                return Modifier::RIGHT_BUTTON;
            case 2:
                return Modifier::MIDDLE_BUTTON;
        }
        return Modifier::NONE;
    }

    void pressed (const PuglButtonEvent& pev, lvtk::View& view) {
        auto& ev  = down[pev.button].set (pev);
        ev.view   = &view;
        ev.source = &view.widget();
        ev.target = ev.source;
        ev.down   = true;
        mods      = mods.with_flags (button_to_flag (ev.event.button));

        auto& rev = up[pev.button];
        if ((pev.time - rev.event.time) > multiple_click_timeout)
            click_count[pev.button] = 0;

        clear_state (rev);
    }

    void released (const PuglButtonEvent& pev, lvtk::View& view) {
        auto& ev  = up[pev.button].set (pev);
        ev.down   = false;
        ev.view   = &view;
        ev.source = &view.widget();
        ev.target = ev.source;
        mods      = mods.with_flags (button_to_flag (ev.event.button));

        auto& dev = down[ev.event.button];
        if ((ev.event.time - dev.event.time) <= multiple_click_timeout) {
            ++click_count[ev.event.button];
        } else {
            click_count[ev.event.button] = 0;
        }
    }

    int num_clicks (int button) const noexcept {
        return click_count[button];
    }

    void set_click_timeout (double to) {
        multiple_click_timeout = to;
    }

    void set_target (int button, lvtk::Widget* widget) {
        down[button].target = widget;
        up[button].target   = widget;
    }

    Modifier modifiers() const noexcept { return mods; }

    lvtk::View* view (int button) const noexcept {
        return down[button].view.lock();
    }

    lvtk::Widget* source (int button) const noexcept {
        return down[button].source;
    }

    lvtk::Widget* target (int button) const noexcept {
        return down[button].target;
    }

private:
    // in seconds
    double multiple_click_timeout = 0.187;
    Modifier mods;
    ButtonState down[LVTK_MAX_BUTTONS];
    ButtonState up[LVTK_MAX_BUTTONS];
    int click_count[LVTK_MAX_BUTTONS];

    inline static void clear_state (ButtonState& ev) {
        ev.event  = {};
        ev.down   = false;
        ev.source = nullptr;
        ev.target = nullptr;
        ev.view   = nullptr;
    };
};

// The actual View impl.
class View {
public:
    View (lvtk::View& o, lvtk::Main& m, lvtk::Widget& w);
    ~View();

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
            puglSetParentWindow (view, (PuglNativeView) parent);
    }

    float scale_factor() const noexcept { return static_cast<float> (pugl_scale); }

    void realize() {
        if (view == nullptr)
            return;
        auto status = puglRealize (view);
        lvtk::ignore (status);
    }

    void drag_pressed_widgets (Point<float> pos) {
        for (int i = 0; i < LVTK_MAX_BUTTONS; ++i) {
            if (buttons.is_down (i))
                if (auto w = buttons.target (i))
                    w->drag (input::event (main, widget, *w, pos));
        }
    }

    void set_focused_widget (lvtk::Widget* widget) {
        if (focused == widget)
            return;
        focused = widget;
    }

    bool widget_is_focused (const lvtk::Widget* widget) const {
        return widget != nullptr && focused == widget;
    }

    template <class Fn>
    static void foreach_widget_recursive (lvtk::Widget& widget, Fn&& f) {
        f (widget);
        for (auto w : widget.impl->widgets)
            foreach_widget_recursive (*w, f);
    }

    template <class Fn>
    void foreach_widget (Fn&& f) {
        foreach_widget_recursive (widget, f);
    }

private:
    friend class lvtk::View;
    friend class lvtk::Main;

    lvtk::View& owner;
    lvtk::Main& main;
    lvtk::Widget& widget;
    PuglView* view { nullptr };
    WeakRef<lvtk::Widget> hovered;
    WeakRef<lvtk::Widget> focused;
    Buttons buttons;
    Keyboard keyboard;

    template <typename T>
    struct ScopedInc {
        explicit ScopedInc (T& val) : value (val), original (val) {}
        ~ScopedInc() { value += inc; }
        T& value;
        const T original;
        const T inc { (T) 1 };
        constexpr operator T() const noexcept { return value; }
    };

    double pugl_scale = 1.0;
    int nconfigures   = 0;
    bool configure_pending { false };

    boost::signals2::signal<void()> sig_idle;

    static PuglStatus configure (View& view, const PuglConfigureEvent& ev) {
        ScopedInc nconfigs (view.nconfigures);

        if (ev.flags & PUGL_IS_HINT) {
            VIEW_DBG ("configure: hint");
            return PUGL_SUCCESS;
        }

        VIEW_DBG4 ("pugl: " << (int) nconfigs << ": configure: " << detail::rect<int> (ev).str());

        auto& widget             = view.widget;
        const bool scale_changed = view.pugl_scale != ev.scale;

        if (scale_changed || nconfigs <= 0) {
            if (scale_changed) {
                view.pugl_scale = ev.scale;
                VIEW_DBG ("pugl: scale changed: " << ev.scale);
            }

            VIEW_DBG ("pugl: resize for widget: " << widget.bounds().str()
                                                  << " to PuglCoord " << (widget.bounds() * ev.scale).str());

#if 0
            auto st = puglSetSize (view.view,
                                   static_cast<PuglSpan> (widget.width() * evscale),
                                   static_cast<PuglSpan> (widget.height() * evscale));
#else
            auto st = puglSetFrame (view.view, detail::frame (widget.bounds() * view.scale_factor()));
#endif
            VIEW_DBG2 ("pugl: puglSetSize: " << puglStrerror (st));
            view.configure_pending = true;
            return PUGL_SUCCESS;
        }

        if (view.configure_pending) {
            view.configure_pending = false;
            VIEW_DBG ("handled pending configure");
        }
        
        view.widget.set_bounds (view.widget.x(),
                                view.widget.y(),
                                int (static_cast<float> (ev.width) / view.scale_factor()),
                                int (static_cast<float> (ev.height) / view.scale_factor()));

        return PUGL_SUCCESS;
    }

    static PuglStatus expose (View& view, const PuglExposeEvent& ev) {
        if (ev.flags & PUGL_IS_HINT) {
            VIEW_DBG ("expose: hint");
            return PUGL_SUCCESS;
        }

        VIEW_DBG4 ("pugl: expose: " << detail::rect<int> (ev).str());

        auto x = (float) ev.x / view.scale_factor();
        auto y = (float) ev.y / view.scale_factor();
        auto w = (float) ev.width / view.scale_factor();
        auto h = (float) ev.height / view.scale_factor();
        auto r = Rectangle<float> { x, y, w, h }.as<int>();

        view.owner.expose (r.intersection (view.owner.bounds().at (0)));
        return PUGL_SUCCESS;
    }

    static PuglStatus create (View& view, const PuglRealizeEvent& ev) {
        view.owner.created();
        puglStartTimer (view.view, 0, 14.0 / 1000.0);
        return PUGL_SUCCESS;
    }

    static PuglStatus destroy (View& view, const PuglUnrealizeEvent& ev) {
        puglStopTimer (view.view, 0);
        view.owner.destroyed();
        return PUGL_SUCCESS;
    }

    // static PuglStatus map (View& view, const PuglMapEvent& ev) {
    //     return PUGL_SUCCESS;
    // }

    // static PuglStatus unmap (View& view, const PuglUnmapEvent& ev) {
    //     return PUGL_SUCCESS;
    // }

    static PuglStatus update (View& view, const PuglUpdateEvent& ev) {
        return PUGL_SUCCESS;
    }

    static PuglStatus close (View& view, const PuglCloseEvent& ev) {
        // dirty setup so the demo can at least quit...
        if (view.main.mode() == Mode::PROGRAM)
            view.main.quit();
        return PUGL_SUCCESS;
    }

    static PuglStatus focus_in (View& view, const PuglFocusEvent&) {
        VIEW_DBG ("gained focus");
        view.widget.grab_focus();
        return PUGL_SUCCESS;
    }

    static PuglStatus focus_out (View& view, const PuglFocusEvent&) {
        VIEW_DBG ("lost focus");
        view.widget.release_focus();
        return PUGL_SUCCESS;
    }

    static PuglStatus key_press (View& view, const PuglKeyEvent& ev) {
        view.keyboard.press (view.owner, ev);
        if (auto f = view.focused.lock()) {
            lvtk::KeyEvent kev (ev.key, Modifier (ev.state));
            f->key_down (kev);
        }
        return PUGL_SUCCESS;
    }

    static PuglStatus key_release (View& view, const PuglKeyEvent& ev) {
        view.keyboard.release (view.owner, ev);
        if (auto f = view.focused.lock()) {
            lvtk::KeyEvent kev (static_cast<int> (ev.key), Modifier (ev.state));
            f->key_up (kev);
        }
        return PUGL_SUCCESS;
    }

    static PuglStatus text (View& view, const PuglTextEvent& ev) {
        if (auto f = view.focused.lock()) {
            TextEvent text (ev.string, ev.state);
            f->text_entry (text);
        }
        return PUGL_SUCCESS;
    }

    static PuglStatus pointer_in (View& view, const PuglCrossingEvent& pev) {
        auto pos = detail::point<float> (pev) / view.scale_factor();

        // if (view.widget.obstructed (pos.as<int>().x, pos.as<int>().y)) {
        if (view.widget.contains (pos)) {
            lvtk::Event ev (view.main,
                            pos,
                            Modifier(),
                            &view.widget,
                            &view.widget,
                            0);
            view.widget.exit (ev);
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus pointer_out (View& view, const PuglCrossingEvent& pev) {
        auto pos = detail::point<float> (pev) / view.scale_factor();

        if (view.widget.contains (pos)) {
            lvtk::Event ev (view.main,
                            pos,
                            Modifier(),
                            &view.widget,
                            &view.widget,
                            0);
            view.widget.enter (ev);
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus motion (View& view, const PuglMotionEvent& ev) {
        const auto pos = detail::point<float> (ev) / view.scale_factor();
        WidgetRef ref  = view.widget.widget_at (pos);

        if (ref.valid()) {
            auto ev = input::event (view.main, view.widget, *ref, pos);
            if (view.hovered != ref) {
                VIEW_DBG3 ("hovered changed: " << ref->name());
                ref->enter (ev);
                view.hovered = ref;
            }

            ref->motion (ev);
        } else if (view.hovered) {
            VIEW_DBG3 ("hovered cleared");
            if (auto h = view.hovered.lock()) {
                auto cpos  = h->convert (&view.widget, pos);
                cpos.x     = std::min (std::max (0.f, cpos.x), (float) h->width());
                cpos.y     = std::min (std::max (0.f, cpos.y), (float) h->height());
                auto event = Event (view.main, cpos, Modifier(), h, h, 0);
                h->exit (event);
            }
            view.hovered = nullptr;
        }

        view.drag_pressed_widgets (pos);

        return PUGL_SUCCESS;
    }

    static PuglStatus button_press (View& view, const PuglButtonEvent& ev) {
        view.buttons.pressed (ev, view.owner);
        auto pos = detail::point<float> (ev) / view.scale_factor();

        if (auto w = view.hovered.lock()) {
            auto event = input::event (
                view.main,
                view.widget,
                *w,
                view.buttons.modifiers(),
                pos,
                view.buttons.num_clicks (ev.button));

            if (w->contains (event.pos)) {
                view.buttons.set_target ((int) ev.button, w);
                w->pressed (event);
            }
        }

        return PUGL_SUCCESS;
    }

    static PuglStatus button_release (View& view, const PuglButtonEvent& ev) {
        view.buttons.released (ev, view.owner);

        // VIEW_DBG2 ("button_release: " << view.buttons.duration() << "s held.");
        // VIEW_DBG2 ("num clicks: " << view.buttons.num_clicks (static_cast<int> (ev.button))
        //                           << " dur: " << view.buttons.delta_time_up_down());

        auto pos     = detail::point<float> (ev) / view.scale_factor();
        auto hovered = view.hovered;

        if (auto w = hovered.lock()) {
            auto event = input::event (view.main,
                                       view.widget,
                                       *w,
                                       view.buttons.modifiers(),
                                       pos,
                                       view.buttons.num_clicks (static_cast<int> (ev.button)));
            if (w->contains (event.pos)) {
                if (w == view.buttons.target (ev.button))
                    w->released (event);
            }
        }

        view.buttons.reset (ev.button);
        return PUGL_SUCCESS;
    }

    static PuglStatus scroll (View& view, const PuglScrollEvent& ev) { return PUGL_SUCCESS; }
    static PuglStatus client (View& view, const PuglClientEvent& ev) { return PUGL_SUCCESS; }

    static PuglStatus timer (View& view, const PuglTimerEvent& ev) {
        view.sig_idle();
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
            CASE2 (PUGL_CONFIGURE, configure) //< View moved/resized, a #PuglConfigureEvent
            CASEA (PUGL_UPDATE, update)       //< View ready to draw, a #PuglUpdateEvent
            CASE2 (PUGL_EXPOSE, expose)       //< View must be drawn, a #PuglExposeEvent
            CASEA (PUGL_REALIZE, create)      //< View created, a #PuglCreateEvent
            CASEA (PUGL_UNREALIZE, destroy)   ///< View destroyed, a #PuglDestroyEvent
            // CASEA (PUGL_MAP, map)                                   ///< View made visible, a #PuglMapEvent
            // CASEA (PUGL_UNMAP, unmap)                               ///< View made invisible, a #PuglUnmapEvent
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
} // namespace lvtk
