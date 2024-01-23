// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>

#include <lvtk/lvtk.h>
#include <lvtk/ui/rectangle.hpp>
#include <lvtk/ui/transform.hpp>

namespace lvtk {

/** How to fit a rectangle within another. This is similar to Justify, but has
    extra options specific to rectangle scale-to-fit logic.
*/
class LVTK_API Fitment {
public:
    inline Fitment (int placement) noexcept : _flags (placement) {}
    inline Fitment() = default;

    Fitment (const Fitment&) = default;

    Fitment& operator= (const Fitment&) = default;

    bool operator== (const Fitment&) const noexcept;
    bool operator!= (const Fitment&) const noexcept;

    enum Flags : uint32_t {
        X_LEFT   = (1u << 0u),
        X_RIGHT  = (1u << 1u),
        X_MIDDLE = (1u << 2u),

        Y_TOP    = (1u << 3u),
        Y_BOTTOM = (1u << 4u),
        Y_MIDDLE = (1u << 5u),

        STRETCH = (1u << 6u),
        FILL    = (1u << 7u),

        ONLY_SHRINK = (1u << 8u),
        ONLY_GROW   = (1u << 9u),

        NO_RESIZE = ONLY_GROW | ONLY_SHRINK,

        CENTERED = X_MIDDLE | Y_MIDDLE
    };

    inline constexpr uint32_t flags() const noexcept { return _flags; }

    void apply_to (double& sx, double& sy, double& sw, double& sh,
                   double dx, double dy, double dw, double dh) const noexcept;

    Transform transform (const Rectangle<float>& source,
                         const Rectangle<float>& destination) const noexcept;

private:
    uint32_t _flags { CENTERED };
};

} // namespace lvtk
