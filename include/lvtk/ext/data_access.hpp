// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/extension.hpp"

#include <lv2/data-access/data-access.h>

namespace lvtk {
/** An LV2_Extension_Data_Feature function wrapper

    Use these on the stack and call set() passing the appropriate feature.
    @headerfile lvtk/ext/data_access.hpp
    @ingroup utility
*/
struct ExtensionData final : FeatureData<LV2_Extension_Data_Feature> {
    ExtensionData() : FeatureData (LV2_DATA_ACCESS_URI) {}

    /** A UI can call this to get data (of a type specified by some other
        extension) from the plugin.

        This call never is never guaranteed to return anything, UIs should
        degrade gracefully if direct access to the plugin data is not possible
        (in which case this function will return NULL).

        This is for access to large data that can only possibly work if the UI
        and plugin are running in the same process.  For all other things, use
        the normal LV2 UI communication system.

        @param uri  The uri string to query
        @returns    Not nullptr on Success
     */
    const void* data_access (const std::string& uri) const {
        return nullptr != data ? data->data_access (uri.c_str())
                               : nullptr;
    }

    const void* operator() (const std::string& uri) const {
        return data_access (uri);
    }
};

/** Give access to plugin extension data to your @ref UI
 
    @tparam Mod Your UI type
    @headerfile lvtk/ext/data_access.hpp
    @ingroup ext
*/
template <class Mod>
struct DataAccess : NullExtension {
    /** @private */
    DataAccess (const FeatureList& features) {
        for (const auto& f : features)
            if (_extension_data.set (f))
                break;
    }

    /** Calls extension_data on the plugin if supported by the host.

        @param uri  URI of the extension
        @note This is an alias to `data_access`

        @returns extension data or nullptr if not available
     */
    inline const void* data_access (const std::string& uri) const {
        return _extension_data.data_access (uri);
    }

private:
    ExtensionData _extension_data;
};
/* @} */
} // namespace lvtk
