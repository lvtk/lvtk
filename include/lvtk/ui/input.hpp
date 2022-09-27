// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

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
#if __APPLE__
        COMMAND = (1u << 3u),
#else
        COMMAND = CTRL,
#endif
        LEFT_BUTTON   = (1u << 4u),
        RIGHT_BUTTON  = (1u << 5u),
        MIDDLE_BUTTON = (1u << 6u),

#if __APPLE__
        POPUP = RIGHT_BUTTON | CTRL,
#else
        POPUP   = RIGHT_BUTTON,
#endif
        /** Alias of CTRL */
        CONTROL = CTRL,

        /** All keyboard Shift, Control, Alt, Command */
        ALL_KEYBOARD = SHIFT | CTRL | ALT | COMMAND,

        /** All buttons left, right, and middle */
        ALL_BUTTON = LEFT_BUTTON | RIGHT_BUTTON | MIDDLE_BUTTON,

        /** All Alt, Control and Command.  */
        CTRL_ALT_COMMAND = CTRL | ALT | COMMAND
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

/** An event type sent about user input.
    @ingroup widgets
*/
struct LVTK_API Event final {
    /** Position of the input when the event occured */
    const Point<float> pos;

    /** X position of the input when the event occured */
    const int x;

    /** Y position of the input when the event occured */
    const int y;

    /** Modifiers associated with the Event. */
    const Modifier mods;

    /** Position where a button was last pressed. */
    const Point<float> down_pos;

    const int num_clicks;

    /** The main context driving the event loop */
    Main* const main;

    /** Widget the event first occured on */
    Widget* const source; ///< The source Widget.

    /** Widget which this event now applies to */
    Widget* const target; ///< The source Widget.

    Event() = delete;

    /** Construct a new event. You shouldn't need this directly.
     
        @param context The context driving the event loop
        @param position Position of the event.
        @param modifiers Current Modifier keys
        @param source_widget The originating widget when the event was created.
        @param target_widget The target widget event is applied to
        @param total_clicks How many clicks have been tracked at the time of creation

        @see Widget
    */
    Event (Main& context, Point<float> position, Modifier modifiers,
           Widget* source_widget, Widget* target_widget,
           int total_clicks)
        : pos (position),
          x (static_cast<int> (pos.x)),
          y (static_cast<int> (pos.y)),
          mods (modifiers),
          down_pos (position),
          num_clicks (total_clicks),
          main (&context),
          source (source_widget),
          target (target_widget) {}

private:
    Event& operator= (const Event& o);
};

} // namespace lvtk
