// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <functional>
#include <lvtk/ui/widget.hpp>

namespace lvtk {

/** A generic button. 
    @ingroup widgets
    @headerfile lvtk/ui/button.hpp
*/
class Button : public lvtk::Widget {
public:
    virtual ~Button() = default;

    /** Executed when the button is clicked */
    std::function<void()> clicked;

    std::function<void (const Event& ev)> __clicked;

    /** Returns true if this button is in a toggled state.
        @returns True when in a toggle state
    */
    bool toggled() const noexcept { return _toggled; }

    /** Toggle or de-toggle this button
     
        @param toggled True if it should be on
    */
    void toggle (bool toggled) {
        if (toggled == _toggled)
            return;
        _toggled = toggled;
        repaint();
    }

protected:
    /** Inherrit Button to write a custom button type */
    Button() = default;

    /** Override to paint your button.
        @param g The graphics to use
        @param highlight True if should be highlighted
        @param down True if currently pressed
    */
    virtual void paint_button (Graphics& g, bool highlight, bool down) {}

    void paint (Graphics& g) override {
        paint_button (g, _over, _down);
    }

    /** @private */
    bool obstructed (int, int) override { return true; }

    /** @private */
    void enter (const Event&) override {
        _over = true;
        repaint();
    }

    /** @private */
    void exit (const Event& ev) override {
        _over = false;
        repaint();
    }

    /** @private */
    void pressed (const Event& ev) override {
        _down = true;
        repaint();
    }

    /** @private */
    void released (const Event& ev) override {
        _down = false;
        if (contains (ev.pos))
            notify_clicked (ev);
        repaint();
    }

private:
    bool _toggled = false;
    bool _down = false, _over = false;

    void notify_clicked (const Event& ev) {
        if (clicked)
            clicked();
        if (__clicked)
            __clicked (ev);
    }
};

/** A button which displays some text */
class TextButton : public Button {
public:
    TextButton() = default;
    TextButton (const String& text) : _text (text) {}
    virtual ~TextButton() = default;

    String text() const noexcept { return _text; }

    void set_text (const String& text) {
        if (text == _text)
            return;
        _text = text;
        repaint();
    }

protected:
    void paint_button (Graphics& g, bool highlight, bool down) override {
        auto& s = style();
        s.draw_button_shape (g, *this, highlight, down);
        s.draw_button_text (g, *this, highlight, down);
    }

private:
    String _text;
};

} // namespace lvtk
