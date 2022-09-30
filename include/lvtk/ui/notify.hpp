// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>

namespace lvtk {

/** A type of notification
    @ingroup widgets
    @headerfile lvtk/ui/slider.hpp
 */
enum class Notify : uint32_t {
    NONE = 0, ///< Don't send any notification
    SYNC,     ///< Send notification sync
    ASYNC     ///< Send notification async
};

} // namespace lvtk
