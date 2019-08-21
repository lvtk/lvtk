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

#include <lvtk/instance_data.hpp>
#include <lvtk/ext/extension.hpp>

namespace lvtk {
/* @{ */
/** Give access to plugin extension data to your @ref UIInstance */
template<class I>
struct DataAccess : NullExtension
{
    /** @private */
    DataAccess (const FeatureList& features) {
        for (const auto& f : features)
            if (instance_data.set_feature (f))
                break;
    }

    /** Calls extension_data on the plugin if supported by the host.
        @param uri  URI of the extension
        @returns extension data or nullptr if not available
     */
    inline const void* plugin_extension_data (const std::string& uri) const {
        return instance_data.data_access (uri.c_str());
    }

private:
    InstanceData instance_data;
};
/* @} */
}
