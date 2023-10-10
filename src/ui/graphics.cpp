// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/graphics.hpp>
#include <lvtk/ui/path.hpp>

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

void Graphics::fill_path (const Path& path) {
    for (const auto& i : path) {
        switch (i.type) {
            case PathOp::MOVE:
                _context.move_to (i.x1, i.y1);
                break;
            case PathOp::LINE:
                _context.line_to (i.x1, i.y1);
                break;
            case PathOp::QUADRATIC:
                _context.quad_to (i.x1, i.y1, i.x2, i.y2);
                break;
            case PathOp::CUBIC:
                _context.cubic_to (i.x1, i.y1, i.x2, i.y2, i.x3, i.y3);
                break;
            case PathOp::CLOSE:
                _context.close_path();
                break;
        }
    }
}

void Graphics::fill_rect (float x, float y, float width, float height) {
    fill_rect (Rectangle<float> (x, y, width, height));
}
void Graphics::fill_rect (int x, int y, int width, int height) {
    fill_rect (Rectangle<int> (x, y, width, height).as<float>());
}
void Graphics::fill_rect (const Rectangle<float>& r) {
    _context.fill_rect (r);
}
void Graphics::fill_rect (const Rectangle<int>& r) {
    _context.fill_rect (r.as<float>());
}

void Graphics::draw_rounded_rect (float x, float y, float width, float height, float corner_size) {
    _context.begin_path();
    graphics::rounded_rect (_context, x, y, width, height, corner_size)
        .stroke();
}

void Graphics::draw_rounded_rect (int x, int y, int width, int height, float corner_size) {
    draw_rounded_rect (static_cast<float> (x),
                       static_cast<float> (y),
                       static_cast<float> (width),
                       static_cast<float> (height),
                       corner_size);
}

void Graphics::draw_rounded_rect (const Rectangle<int>& r, float corner_size) {
    draw_rounded_rect (r.as<float>(), corner_size);
}

void Graphics::draw_rounded_rect (const Rectangle<float>& r, float corner_size) {
    draw_rounded_rect (r.x, r.y, r.width, r.height, corner_size);
}

void Graphics::fill_rounded_rect (float x, float y, float width, float height, float corner_size) {
    _context.begin_path();
    graphics::rounded_rect (_context, x, y, width, height, corner_size)
        .fill();
}

void Graphics::fill_rounded_rect (int x, int y, int width, int height, float corner_size) {
    fill_rounded_rect (
        static_cast<float> (x),
        static_cast<float> (y),
        static_cast<float> (width),
        static_cast<float> (height),
        corner_size);
}

void Graphics::fill_rounded_rect (const Rectangle<int>& r, float cs) {
    fill_rounded_rect (r.as<float>(), cs);
}

void Graphics::fill_rounded_rect (const Rectangle<float>& r, float corner_size) {
    fill_rounded_rect (r.x, r.y, r.width, r.height, corner_size);
}

void Graphics::draw_text (const std::string& text, Rectangle<float> area, Alignment align) {
    float x                 = area.x;
    float y                 = area.y;
    const float line_height = 16.f;

    if (align.flags() & Align::CENTER)
        x = area.x + (area.width * 0.5f);
    else if (align.flags() & Align::RIGHT)
        x = area.x + area.width;
    if (align.flags() & Align::MIDDLE)
        y = area.y + (area.height * 0.5f);
    else if (align.flags() & Align::BOTTOM)
        y = area.y + area.height - line_height;

    _context.text (text, x, y, align);
};

/** Returns the context used by this Graphics instance. */
DrawingContext& Graphics::context() { return _context; }

} // namespace lvtk
