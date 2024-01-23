// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint> // for uint8_t
#include <utility> // for std::move

namespace lvtk {

/** A utility class to help aligning text within its container. This is similar
    to Fitment, but does not imply any kind of scale-to-fit logic.

    @ingroup graphics
    @headerfile lvtk/ui/justify.hpp
*/
class Justify final {
public:
    enum Flags : uint8_t {
        LEFT   = (1u << 0u), ///< Align left.
        CENTER = (1u << 2u), ///< Align horizontally centered.
        RIGHT  = (1u << 1u), ///< Align right.

        TOP    = (1u << 3u), ///< Align to top.
        MIDDLE = (1u << 5u), ///< Align vertically centered.
        BOTTOM = (1u << 4u), ///< Align to bottom.

        /// @brief Align to top-left corner
        TOP_LEFT = TOP | LEFT,
        /// @brief Align center at the top
        TOP_CENTER = TOP | CENTER,
        /// @brief Align to top-right corner
        TOP_RIGHT = TOP | RIGHT,

        /// @brief Aligned left in the middle
        LEFT_MIDDLE = LEFT | MIDDLE,
        /// @brief Align centered vertically and horizontally
        CENTERED = CENTER | MIDDLE,
        /// @brief Aligned right in the middle
        RIGHT_MIDDLE = RIGHT | MIDDLE,

        /// @brief Align to bottom-left corner
        BOTTOM_LEFT = BOTTOM | LEFT,
        /// @brief Align center at the bottom
        BOTTOM_CENTER = BOTTOM | CENTER,
        /// @brief Align to bottom-right corner
        BOTTOM_RIGHT = BOTTOM | RIGHT
    };

    /** Create an invalid alignment */
    Justify() {}
    /** Create an alignment from flags */
    Justify (uint8_t flags) : _flags (flags) {}
    /** Copy this alignment */
    Justify (const Justify& o) : _flags (o._flags) {}
    Justify& operator= (const Justify& o) {
        _flags = o._flags;
        return *this;
    }

    /** Returns the flags of this Align */
    uint8_t flags() const noexcept { return _flags; }

    bool operator== (const Justify& o) const noexcept { return _flags == o._flags; }
    bool operator== (uint8_t o) const noexcept { return _flags == o; }
    bool operator== (int o) const noexcept { return static_cast<int> (_flags) == o; }

    bool operator!= (const Justify& o) const noexcept { return _flags != o._flags; }
    bool operator!= (uint8_t o) const noexcept { return _flags != o; }
    bool operator!= (int o) const noexcept { return static_cast<int> (_flags) != o; }

private:
    uint8_t _flags = 0;
};

} // namespace lvtk
