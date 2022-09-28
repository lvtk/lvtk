// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <sstream>
#include <string>

#include <lvtk/lvtk.h>

#include <lvtk/ui/graphics.hpp> // point
#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Main;
class View;
class Widget;

/** alias for Modifier flags */
using ModifierFlags = uint32_t;

/** Modifier keys
    @ingroup widgets
*/
class LVTK_API Modifier final {
public:
    enum Flag : ModifierFlags {
        NONE  = 0u,
        SHIFT = (1u << 0u),
        CTRL  = (1u << 1u),
        ALT   = (1u << 2u),
        SUPER = (1u << 3u),
#if __APPLE__
        /** Alias of SUPER on mac, or alias of CTRL everywhere else. */
        COMMAND = SUPER,
#else
        COMMAND = CTRL,
#endif

        /** Alias of CTRL */
        CONTROL = CTRL,

        /** All Alt, Control and Command.  */
        CTRL_ALT_COMMAND = CTRL | ALT | COMMAND,

        /** All keyboard Shift, Control, Alt, Command */
        ALL_KEYBOARD = SHIFT | CTRL | ALT | COMMAND,

        /** Left button down */
        LEFT_BUTTON = (1u << 4u),
        /** Right button down */
        RIGHT_BUTTON = (1u << 5u),
        /** Middle button down */
        MIDDLE_BUTTON = (1u << 6u),

#if __APPLE__
        /** Indicates a popup event.
            i.e. right-click on win/linux/osx. Also includes
            CTRL + left-click when using on OSX
        */
        POPUP = RIGHT_BUTTON | CTRL,
#else
        POPUP   = RIGHT_BUTTON,
#endif

        /** All buttons left, right, and middle */
        ALL_BUTTON = LEFT_BUTTON | RIGHT_BUTTON | MIDDLE_BUTTON,

    };

    Modifier() {}
    Modifier (const Modifier& o) : _flags (o._flags) {}
    Modifier (uint32_t f) : _flags (f) {}

    Modifier& operator= (const Modifier& o) {
        _flags = o._flags;
        return *this;
    }

    ModifierFlags flags() const noexcept { return _flags; }
    Modifier with_flags (ModifierFlags raw) const noexcept { return Modifier (_flags | raw); }
    Modifier without_flags (ModifierFlags raw) const noexcept { return Modifier (_flags & ~raw); }
    bool test_flags (ModifierFlags raw) const noexcept { return (_flags & raw) != 0; }

    /** Returns true if this could be used for a popup menu.
        - Windows/Linux tests for a right click.
        - Mac, tests for either the CTRL key being down, or right click.
        @returns true if a popup menu
     */
    bool popup() const noexcept { return (_flags & POPUP) != 0; }

    /** Returns true if the left button is down. */
    bool left_button() const noexcept { return (_flags & LEFT_BUTTON) != 0; }

    /** Returns true if the right button is down */
    bool right_button() const noexcept { return (_flags & RIGHT_BUTTON) != 0; }

    /** Returns true if the right button is down */
    bool middle_button() const noexcept { return (_flags & RIGHT_BUTTON) != 0; }

    /** Returns how many buttons are currently held down. */
    int num_buttons() const noexcept {
        int count = 0;

        if ((_flags & LEFT_BUTTON))
            ++count;
        if ((_flags & RIGHT_BUTTON))
            ++count;
        if ((_flags & MIDDLE_BUTTON))
            ++count;

        return count;
    }

private:
    ModifierFlags _flags { NONE };
};

/** A KeyPress event */
class LVTK_API KeyEvent {
public:
    KeyEvent() {}
    KeyEvent (int key) : _key (key) {}
    KeyEvent (int key, Modifier mods) : _key (key), _mods (mods) {}

    int key() const noexcept { return _key; }
    Modifier mods() const noexcept { return _mods; }

    std::string str() const noexcept {
        std::vector<std::string> toks;
        toks.reserve (4);
        std::stringstream out;

        if (_mods.test_flags (Modifier::CTRL))
            toks.push_back ("Ctrl");
        if (_mods.test_flags (Modifier::ALT))
            toks.push_back ("Alt");
        if (_mods.test_flags (Modifier::SHIFT))
            toks.push_back ("Shift");

        for (const auto& tok : toks) {
            out << tok << " + ";
        }

        out << std::to_string ((char) std::toupper ((int) _key));

        return out.str();
    }

private:
    int _key { 0 };
    Modifier _mods;
};

/** A KeyPress event */
class LVTK_API TextEvent {
public:
    TextEvent() : character (0) {}
    TextEvent (const char* txt, Modifier m)
        : mods (m),
          character (static_cast<uint32_t> (txt[0])),
          body (txt) {}

    const Modifier mods;
    const uint32_t character;
    const std::string body;
};

/** An event type sent about user input.
    @ingroup widgets
*/
struct LVTK_API Event final {
    /** Position of the input when the event occured (high res) */
    const Point<float> pos;

    /** X position of the input when the event occured */
    const int x;

    /** Y position of the input when the event occured */
    const int y;

    /** Modifiers associated with the Event. */
    const Modifier mods;

    /** Position where a button was last pressed. */
    const Point<float> down_pos;

    /** Number of clicks associated with the event. */
    const int clicks;

    /** The main context driving the event loop */
    Main& context;

    /** Widget the event first occured on */
    Widget* const source;

    /** Widget which this event now applies to */
    Widget* const target;

    Event() = delete;

    /** Construct a new event. You shouldn't need to use this directly.
     
        @param ctx The context driving the event loop
        @param position Position of the event.
        @param modifiers Current Modifier keys
        @param source_widget The originating widget when the event was created.
        @param target_widget The target widget event is applied to
        @param num_clicks How many clicks have been tracked at the time of creation

        @see Widget
    */
    Event (Main& ctx, Point<float> position, Modifier modifiers,
           Widget* source_widget, Widget* target_widget,
           int num_clicks)
        : pos (position),
          x (static_cast<int> (pos.x)),
          y (static_cast<int> (pos.y)),
          mods (modifiers),
          down_pos (position),
          clicks (num_clicks),
          context (ctx),
          source (source_widget),
          target (target_widget) {}

private:
    Event& operator= (const Event& o);
};

} // namespace lvtk
