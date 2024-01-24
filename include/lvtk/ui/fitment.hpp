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
        /** Snap left edge of source to left edge of target. */
        LEFT = (1u << 0u),
        /** Snap the right edge of the source to the right edge of the target. */
        RIGHT = (1u << 1u),
        /** Center the source horizontally in the target. */
        MID_X = (1u << 2u),

        /** Snap the top edge of the source to the top edge of the target. */
        TOP = (1u << 3u),
        /** Snap the bottom edge of the source to the bottom edge of the target. */
        BOTTOM = (1u << 4u),
        /** Center the source vertically in the target. */
        MID_Y = (1u << 5u),

        /** Center the source horizontally and vertically in the target. */
        CENTERED = MID_X | MID_Y,

        /** Stretch the source to fit. */
        STRETCH = (1u << 6u),
        /** Fill the full frame. */
        FILL = (1u << 7u),

        /** Don't expand. */
        NO_GROW = (1u << 8u),
        /** Don't shrink. */
        NO_SHRINK = (1u << 9u),

        /** Only get larger. Same as NO_SHRINK */
        ONLY_GROW = NO_SHRINK,
        /** Only get smaller. Same as NO_GROW */
        ONLY_SHRINK = NO_GROW,

        /** Don't resize. */
        NO_RESIZE = NO_SHRINK | NO_GROW
    };

    inline constexpr flag_type flags() const noexcept { return _flags; }

    void apply (double& sx, double& sy, double& sw, double& sh,
                double dx, double dy, double dw, double dh) const noexcept;

    Transform transform (const Rectangle<double>& src,
                         const Rectangle<double>& dst) const noexcept;

private:
    flag_type _flags { CENTERED };
};

} // namespace lvtk
