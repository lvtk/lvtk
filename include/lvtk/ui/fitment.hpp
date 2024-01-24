// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/justify.hpp>
#include <lvtk/ui/rectangle.hpp>
#include <lvtk/ui/transform.hpp>

namespace lvtk {

/** Flags to use when fitting a rectangle within another. This is an extension 
    to Justify adding options specific to rectangle scale-to-fit logic. The flag
    set starts where Justify leaves off and the two can be mixed.
*/
struct LVTK_API Fitment {
    using flag_type = int;

    /** Makes a fitment. 
        @param placement Fitment + Justify flags
    */
    inline Fitment (int placement) noexcept : _flags (placement) {}
    inline Fitment() = default;

    Fitment (const Fitment&) = default;

    Fitment& operator= (const Fitment&) = default;
    Fitment& operator= (int placement) noexcept {
        _flags = placement;
        return *this;
    }

    bool operator== (const Fitment&) const noexcept;
    bool operator!= (const Fitment&) const noexcept;

    enum : flag_type {
        LEFT     = (1u << 0u),
        RIGHT    = (1u << 1u),
        MID_X    = (1u << 2u),
        TOP      = (1u << 3u),
        BOTTOM   = (1u << 4u),
        MID_Y    = (1u << 5u),
        CENTERED = MID_X | MID_Y,

        STRETCH = (1u << 6u),
        FILL    = (1u << 7u),

        NO_GROW   = (1u << 8u),
        NO_SHRINK = (1u << 9u),

        NO_RESIZE = NO_SHRINK | NO_GROW
    };

    inline constexpr flag_type flags() const noexcept { return _flags; }

    void apply_to (double& sx, double& sy, double& sw, double& sh,
                   double dx, double dy, double dw, double dh) const noexcept;

    Transform transform (const Rectangle<float>& src,
                         const Rectangle<float>& dst) const noexcept;

private:
    flag_type _flags { CENTERED };
};

} // namespace lvtk
