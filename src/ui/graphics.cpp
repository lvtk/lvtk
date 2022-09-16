// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/graphics.hpp>

namespace lvtk {
Graphics::Graphics (DrawingContext& d)
    : surface (d) {}

void Graphics::set_font (const Font& font) { surface.set_font (font); }
void Graphics::set_font_height (float height) {
    surface.set_font (surface.font().with_height (height));
}

void Graphics::draw_text (const std::string& text, Rectangle<float> area, Alignment align) {
    float x                 = area.x;
    float y                 = area.y;
    const float line_height = 16.f;

    if (align.flags() & Alignment::CENTER)
        x = area.x + (area.width * 0.5f);
    else if (align.flags() & Alignment::RIGHT)
        x = area.x + area.width;
    if (align.flags() & Alignment::MIDDLE)
        y = area.y + (area.height * 0.5f);
    else if (align.flags() & Alignment::BOTTOM)
        y = area.y + area.height - line_height;

    surface.text (text, x, y, align);
};

} // namespace lvtk
