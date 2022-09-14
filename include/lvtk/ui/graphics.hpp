// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/color.hpp>
#include <lvtk/ui/fill.hpp>
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
    void save() { surface.save(); }
    void restore() { surface.restore(); }

    void set_color (Color color) { surface.set_fill (color); }
    void fill_rect (const Rectangle<float>& r) { surface.fill_rect (r); }
    void fill_rect (const Rectangle<int>& r) { surface.fill_rect (r.as<float>()); }

    void clip (Bounds c) { surface.clip (c); }
    void intersect_clip (Bounds c) { surface.intersect_clip (c); }
    Bounds last_clip() const noexcept { return surface.last_clip(); }
    bool clip_empty() const noexcept { return surface.last_clip().empty(); }

    void text (const std::string& text, float x, float y) {
        surface.__text_top_left (text, x, y);
    }

private:
    Surface& surface;
};

} // namespace lvtk
