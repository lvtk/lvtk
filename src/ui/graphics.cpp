// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/graphics.hpp>

namespace lvtk {

Graphics::Graphics (DrawingContext& d)
    : _context (d) {}

void Graphics::translate (const Point<int>& delta) { _context.translate (delta); }
void Graphics::clip (Bounds c) { _context.clip (c); }
void Graphics::intersect_clip (Bounds c) { _context.intersect_clip (c); }
Bounds Graphics::last_clip() const noexcept { return _context.last_clip(); }
bool Graphics::clip_empty() const noexcept { return _context.last_clip().empty(); }

void Graphics::save() { _context.save(); }
void Graphics::restore() { _context.restore(); }

void Graphics::set_font (const Font& font) { _context.set_font (font); }

void Graphics::set_color (Color color) { _context.set_fill (color); }

void Graphics::fill_rect (const Rectangle<float>& r) { _context.fill_rect (r); }
void Graphics::fill_rect (const Rectangle<int>& r) { _context.fill_rect (r.as<float>()); }

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

    _context.text (text, x, y, align);
};

/** Returns the context used by this Graphics instance. */
DrawingContext& Graphics::context() { return _context; }

} // namespace lvtk
