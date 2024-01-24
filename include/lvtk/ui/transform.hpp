// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cmath>

#include <lvtk/lvtk.h>

namespace lvtk {

/** An affine transformation matrix.
  
    @ingroup graphics
    @headerfile lvtk/ui/graphics.hpp
*/
class LVTK_API Transform final {
public:
    Transform() = default;

    bool operator== (const Transform& o) const noexcept {
        return m00 == o.m00
               && m01 == o.m01
               && m02 == o.m02
               && m10 == o.m10
               && m11 == o.m11
               && m12 == o.m12;
    }

    bool operator!= (const Transform& o) const noexcept {
        return ! operator== (o);
    }

    float m00 { 1.f }, ///> The xx
        m01 { 0.f },   ///> The xy
        m02 { 0.f },   ///> The x0
        m10 { 0.f },   ///> The yx
        m11 { 1.f },   ///> The yy
        m12 { 0.f };   ///> The y0

    // clang-format off
    static Transform rotation (float angle) noexcept {
        auto angle_cos = std::cos (angle);
        auto angle_sin = std::sin (angle);
        return { angle_cos, -angle_sin, 0,
                 angle_sin,  angle_cos, 0 };
    }

    static Transform translation (float dx, float dy) noexcept {
        return { 1.0f, 0.0f, dx, 
                 0.0f, 1.0f, dy };
    }

    Transform translated (float dx, float dy) const noexcept {
        return { m00, m01, m02 + dx, 
                 m10, m11, m12 + dy };
    }

    Transform scaled (float scale_x, float scale_y) const noexcept {
        return { scale_x * m00, scale_x * m01, scale_x * m02, 
                 scale_y * m10, scale_y * m11, scale_y * m12 };
    }

    Transform scaled (float factor) const noexcept {
        return { factor * m00, factor * m01, factor * m02, 
                 factor * m10, factor * m11, factor * m12 };
    }
    // clang-format on
};

} // namespace lvtk