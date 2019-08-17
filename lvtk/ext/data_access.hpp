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

#pragma once

#include <lvtk/lvtk.hpp>
#include <lv2/lv2plug.in/ns/ext/data-access/data-access.h>

namespace lvtk {

/** Wrap Data access host feature
    @headerfile lvtk/ext/data_access.hpp
*/
struct DataAccess
{
    DataAccess()
    {
        memset (&m_data_access, 0, sizeof(LV2_Extension_Data_Feature));
    }

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
        return nullptr != m_data_access.data_access 
            ? m_data_access.data_access (uri.c_str())
            : nullptr;
    }

    /** Alias to data_access() */
    const void* get_data (const std::string& uri) const {
        return data_access (uri);
    }

    /** Set extension data from an LV2 Feature */
    void set_feature (const Feature& feature) {
        m_data_access = *(LV2_Extension_Data_Feature*) feature.data;
    }

private:
    /** @internal Feature Data passed from host */
    LV2_Extension_Data_Feature m_data_access { nullptr };
};

} /* namespace lvtk */
