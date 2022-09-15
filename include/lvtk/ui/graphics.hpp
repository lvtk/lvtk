// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/alignment.hpp>
#include <lvtk/ui/color.hpp>
#include <lvtk/ui/fill.hpp>
#include <lvtk/ui/font.hpp>
#include <lvtk/ui/image.hpp>
#include <lvtk/ui/rectangle.hpp>
#include <lvtk/ui/surface.hpp>

namespace lvtk {

using Bounds = Rectangle<int>;

/** Higher level graphics context.
    API is subject to change dramatically at any given time
    until we approach an alpha status.
 */
class Graphics final {
public:
    Graphics (Surface& d);
    ~Graphics() = default;

    void translate (const Point<int>& delta) { surface.translate (delta); }
    void clip (Bounds c) { surface.clip (c); }
    void intersect_clip (Bounds c) { surface.intersect_clip (c); }
    Bounds last_clip() const noexcept { return surface.last_clip(); }
    bool clip_empty() const noexcept { return surface.last_clip().empty(); }

    void save() { surface.save(); }
    void restore() { surface.restore(); }

    void set_font (const Font& font);
    void set_font_height (float height);

    void set_color (Color color) { surface.set_fill (color); }

    void fill_rect (const Rectangle<float>& r) { surface.fill_rect (r); }
    void fill_rect (const Rectangle<int>& r) { surface.fill_rect (r.as<float>()); }

    /** Draw some text */
    void draw_text (const std::string& text, Rectangle<float> area, Alignment align);

    /** Returns the context used by this Graphics instance. */
    Surface& context() { return surface; }

private:
    Surface& surface;
};

} // namespace lvtk
