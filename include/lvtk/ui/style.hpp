// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <set>

#include <lvtk/ui/color.hpp>
#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Button;
class Graphics;
class TextButton;
class Widget;

using ColorID = int;

enum class ColorIDs : ColorID {
    BUTTON_BASE = 1,
    BUTTON_ON,
    BUTTON_TEXT_OFF,
    BUTTON_TEXT_ON
};

class Style {
public:
    Style() {
        _weak_status.reset (this);
    }

    virtual ~Style() {
        _weak_status.reset();
    }

    void set_color (int ID, Color color) {
        _colors.insert (ColorItem {ID, color});
    }
    
    Color find_color (int ID) const noexcept {
        if (_colors.empty())
            return {};
        auto it = _colors.find (ColorItem {ID, {}});
        if (it != _colors.end())
            return (*it).color;
        return {};
    }

    virtual void draw_button_shape (Graphics& g, Button& w, bool highlight, bool down) = 0;
    virtual void draw_button_text (Graphics& g, TextButton& b, bool highlight, bool down) =0;

private:
    struct ColorItem {
        int ID;
        Color color;
        bool operator== (const ColorItem& o) const noexcept { return ID == o.ID; }
        bool operator< (const ColorItem& o) const noexcept { return ID < o.ID; }
        bool operator> (const ColorItem& o) const noexcept { return ID > o.ID; }
    };

    std::set<ColorItem> _colors;
    LVTK_WEAK_REFABLE (Style);
};

} // namespace lvtk
