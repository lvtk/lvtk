// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <set>

#include <lvtk/lvtk.h>
#include <lvtk/ui/color.hpp>
#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Button;
class Graphics;
class TextButton;
class Widget;

/** ColorID's for theming
    @ingroup widgets
    @headerfile lvtk/ui/style.hpp
*/
struct ColorID {
    enum Key : int {
        BUTTON_BASE = 1,    ///< Base background color of standard buttons
        BUTTON_ON,          ///< Base on color when button is "toggled"
        BUTTON_TEXT_OFF,    ///< Base off color when button is not "toggled"
        BUTTON_TEXT_ON      ///< Button text color when ON
    };
};

/** Style used to draw common widgets.
    @ingroup widgets
    @headerfile lvtk/ui/style.hpp
*/
class LVTK_API Style {
public:
    Style();

    virtual ~Style();

    /** Set a color by ID
        @param ID The ColorID to set.
        @param color The Color to associate.
    */
    void set_color (int ID, Color color);

    /** Find a mapped color ID.
        @param ID the ColorID to look for
        @returns The Color.  Will be 0 if not found
    */
    Color find_color (int ID) const noexcept;

    // clang-format off

    /** Override to draw the background shape of a typical button 
        @param g Graphics to draw with
        @param widget The TextButton being rendered
        @param highlight True if down or over
        @param down True if down
    */
    virtual void draw_button_shape (Graphics& g, Button& widget, bool highlight, bool down) =0;
    
    /** Override to draw the text of a TextButton.
        @param g Graphics to draw with
        @param b The TextButton being rendered
        @param highlight True if down or over
        @param down True if down
    */
    virtual void draw_button_text (Graphics& g, TextButton& b, bool highlight, bool down) =0;
    // clang-format on

private:
    struct ColorItem {
        int ID;
        Color color;
        bool operator== (const ColorItem& o) const noexcept { return ID == o.ID; }
        bool operator<(const ColorItem& o) const noexcept { return ID < o.ID; }
        bool operator> (const ColorItem& o) const noexcept { return ID > o.ID; }
    };

    std::set<ColorItem> _colors;
    LVTK_WEAK_REFABLE (Style);
    LVTK_DISABLE_COPY (Style);
};

} // namespace lvtk
