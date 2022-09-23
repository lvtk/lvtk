// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/lvtk.hpp"

namespace lvtk {

/** Base class for all extension mixins.

    You shouldn't need to use this directly, unless you'd like to write your
    own Extension mixins.

    @headerfile lvtk/ext/extension.hpp
    @defgroup ext Extension
    @{
*/
template <class I>
struct Extension {};

/** Dummy class indicating an Extension doesn't use the Instance */
struct NoInstance {};

/** This is for extensions which do not provide extension data
    and/or call methods on the Instance
 */
struct NullExtension : Extension<NoInstance> {
    /** @private */
    inline static void map_extension_data (ExtensionMap&) {}
};

/**
    @}
 */
} // namespace lvtk
