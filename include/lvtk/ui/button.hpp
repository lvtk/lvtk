// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <functional>
#include <lvtk/ui/widget.hpp>

namespace lvtk {
namespace detail {
class Button;
class TextButton;
} // namespace detail

/** A generic button. 
    @ingroup widgets
    @headerfile lvtk/ui/button.hpp
*/
class LVTK_API Button : public lvtk::Widget {
public:
    virtual ~Button();

    /** Executed when the button is clicked */
    std::function<void()> on_clicked;

    /** Returns true if this button is in a toggled state.
        @returns bool True when in a toggle state
    */
    bool toggled() const noexcept;

    /** Toggle or de-toggle this button
        @param toggled True if it should be on
    */
    void toggle (bool toggled);

protected:
    /** Inherrit Button to write a custom button type */
    Button();

    /** Override to paint your button.
        @param g The graphics to use
        @param highlight True if should be highlighted
        @param down True if currently pressed
    */
    virtual void paint_button (Graphics& g, bool highlight, bool down) {}

    /** @private */
    void paint (Graphics& g) override;
    /** @private */
    bool obstructed (int, int) override { return true; }
    /** @private */
    void enter (const Event&) override;
    /** @private */
    void exit (const Event& ev) override;
    /** @private */
    void pressed (const Event& ev) override;
    /** @private */
    void released (const Event& ev) override;

private:
    friend class detail::Button;
    std::unique_ptr<detail::Button> impl;
};

/** A button which displays some text */
class LVTK_API TextButton : public Button {
public:
    TextButton();
    TextButton (const String& text);
    virtual ~TextButton();

    String text() const noexcept;

    void set_text (const String& text);

protected:
    void paint_button (Graphics& g, bool highlight, bool down) override {
        auto& s = style();
        s.draw_button_shape (g, *this, highlight, down);
        s.draw_button_text (g, *this, highlight, down);
    }

private:
    friend class detail::TextButton;
    std::unique_ptr<detail::TextButton> impl;
    LVTK_DISABLE_COPY (TextButton)
};

} // namespace lvtk
