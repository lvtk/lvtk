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

struct ColorID {
    enum : int {
        BUTTON_BASE = 1,
        BUTTON_ON,
        BUTTON_TEXT_OFF,
        BUTTON_TEXT_ON
    };
};

class LVTK_API Style {
public:
    Style();

    virtual ~Style();

    void set_color (int ID, Color color);

    Color find_color (int ID) const noexcept;

    // clang-format off
    virtual void draw_button_shape (Graphics& g, Button& w, bool highlight, bool down) =0;
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
