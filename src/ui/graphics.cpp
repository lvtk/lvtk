// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/graphics.hpp>
#include <lvtk/ui/path.hpp>

#include <iostream>

namespace lvtk {

Graphics::Graphics (DrawingContext& d)
    : _context (d) {}

DrawingContext& Graphics::context() { return _context; }

void Graphics::translate (Point<int> delta) {
    _context.translate (delta.x, delta.y);
}

void Graphics::clip (Bounds c) { _context.clip (c); }
void Graphics::exclude_clip (Bounds c) { _context.exclude_clip (c); }
Bounds Graphics::last_clip() const noexcept { return _context.last_clip(); }
bool Graphics::clip_empty() const noexcept { return _context.last_clip().empty(); }

void Graphics::save() { _context.save(); }
void Graphics::restore() { _context.restore(); }

void Graphics::set_font (const Font& font) { _context.set_font (font); }
void Graphics::set_font (double height) { set_font (Font (static_cast<float> (height))); }

void Graphics::set_color (Color color) { _context.set_fill (color); }

void Graphics::fill_path (const Path& path) {
    _context.begin_path();

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

    _context.fill();
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

void Graphics::stroke_path (const Path& path) {
    _context.begin_path();

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
    _context.stroke();
}

void Graphics::draw_text (const std::string& text, Rectangle<float> area, Justify align) {
    if (text.empty() || area.empty())
        return;

    const auto fe = _context.font_metrics();
    const auto te = _context.text_metrics (text);
    float x       = area.x;
    float y       = area.y;

    if (align.flags() & Justify::MID_X)
        x = (area.x + te.x_offset) + (area.width * 0.5f) - (te.width * 0.5f);
    else if (align.flags() & Justify::RIGHT)
        x = area.x + area.width - te.width;
    if (align.flags() & Justify::TOP)
        y += fe.ascent;
    else if (align.flags() & Justify::MID_Y)
        y = (area.height * 0.5f) - (fe.height * 0.5) + fe.ascent;
    else if (align.flags() & Justify::BOTTOM)
        y = area.y + area.height - fe.descent;

    _context.move_to (x, y);
    _context.show_text (text);
};

void Graphics::draw_image (Image image, Rectangle<double> target, Fitment align) {
    if (! image.valid())
        return;
    draw_image (image, align.transform (image.bounds().as<double>(), target.as<double>()));
}

void Graphics::draw_image (Image image, Transform transform) {
    _context.save();
    _context.draw_image (image, transform);
    _context.restore();
}

} // namespace lvtk
