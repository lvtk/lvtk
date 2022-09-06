// Copyright 2022 Michael Fisher <mfisher@kushview.net>
// SPDX-License-Identifier: ISC

#pragma once

#include <cassert>
#include <cstdint>
#include <sstream>
#include <vector>

#include <lvtk/ui/color.hpp>
#include <lvtk/ui/fill.hpp>
#include <lvtk/ui/image.hpp>
#include <lvtk/ui/rectangle.hpp>
#include <lvtk/ui/surface.hpp>

namespace lvtk {

using Bounds = Rectangle<int>;

class Graphics final {
public:
    Graphics (Surface& d)
        : surface (d) {}

    ~Graphics() = default;

    void translate (const Point<int>& delta) { surface.translate (delta); }
    void save() { surface.save(); }
    void restore() { surface.restore(); }

    void set_color (Color color) { surface.set_fill (color); }
    void fill_rect (const Rectangle<float>& r) { surface.fill_rect (r); }
    void fill_rect (const Rectangle<int>& r) { surface.fill_rect (r.as<float>()); }
    Bounds clip_bounds() const noexcept { return surface.clip_bounds(); }

private:
    Surface& surface;
};

} // namespace lvtk
