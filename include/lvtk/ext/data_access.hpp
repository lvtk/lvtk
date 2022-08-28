/* 
    Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/** @defgroup data_access Data Access 
    Access to plugin extension data
*/

#pragma once

#include <lv2/data-access/data-access.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {
/** An LV2_Extension_Data_Feature function wrapper
 
    Use these on the stack and call set_feature() passing the appropriate feature.
    @headerfile lvtk/ext/data_access.hpp
    @ingroup data_access
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
    const void* operator() (const std::string& uri) const {
        return nullptr != data ? data->data_access (uri.c_str())
                               : nullptr;
    }
};

/** Give access to plugin extension data to your @ref UI
    @headerfile lvtk/ext/data_access.hpp
    @ingroup data_access
*/
template <class I>
struct DataAccess : NullExtension {
    /** @private */
    DataAccess (const FeatureList& features) {
        for (const auto& f : features)
            if (data_access.set (f))
                break;
    }

    /** Calls extension_data on the plugin if supported by the host.
        
        @param uri  URI of the extension
        @note This is an alias to `data_access`

        @returns extension data or nullptr if not available
     */
    inline const void* plugin_extension_data (const std::string& uri) const {
        return data_access (uri);
    }

protected:
    /** The data access function */
    ExtensionData data_access;
};
/* @} */
} // namespace lvtk
