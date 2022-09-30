// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/color.hpp>
#include <lvtk/ui/image.hpp>

namespace lvtk {

/** A type of Fill used by DrawingContext
    i.e. fill Solid, Image, Gradient.
    @ingroup graphics
    @headerfile lvtk/ui/fill.hpp
 */
class LVTK_API Fill {
public:
    Fill() = default;
    void set_color (Color c) {
        _color = c;
    }

    Color color() const noexcept { return _color; }
    bool is_color() const noexcept { return true; }

    Fill (const Color& color) {
        set_color (color);
    }

    Fill (const Fill& o) { operator= (o); }
    Fill& operator= (const Fill& o) {
        _color = o._color;
        return *this;
    }

private:
    Color _color;
};

} // namespace lvtk
