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

#include <cstdlib>
#include <map>
#include <vector>

#include <lv2/core/lv2.h>
#include <lv2/urid/urid.h>
#include <lv2/worker/worker.h>

namespace lvtk {

using Handle        = LV2_Handle;
using URID          = LV2_URID;
using String        = std::string;
using StringArray   = std::vector<String>;
using ExtensionMap  = std::map<String, const void*>;

struct Feature : LV2_Feature {
    Feature() = default;

    Feature (const LV2_Feature& feature)
    {
        data = feature.data;
        URI  = feature.URI;
    }

    inline bool operator== (const char* uri)  const { return strcmp (uri, URI) == 0; }
    inline bool operator== (const String& uri) const { return strcmp (uri.c_str(), URI) == 0; }
};

template<class D>
struct DescriptorList final : public std::vector<D>
{
    inline ~DescriptorList() {
        for (const auto& desc : *this)
            std::free ((void*) desc.URI);
    }
};

struct Extension  {
    Extension (const std::string& _uri) 
        : URI (_uri) {}
    virtual ~Extension() = default;
    const std::string URI;

    inline bool operator== (const char* str_uri) const { return URI == str_uri; }
    inline bool operator!= (const char* str_uri) const { return URI != str_uri; }
};

/** Template class which can be used to assign feature data in a common way.
    Typically these are used to facilitate features provided by the host 
    during plugin instantiation or in host-side callbacks to the plugin.
 */
template<class DataType, bool StorePtr = false>
struct StackExtension : Extension
{
    using DataTypePtr  = DataType*;

    StackExtension() = delete;

    explicit StackExtension (const char* feature_uri)
        : Extension (feature_uri) { }

    ~StackExtension() = default;

    inline void set_feature (const Feature& feature) {
        if (URI == feature.URI) {
            if (StorePtr) {
                data_ptr = static_cast<DataTypePtr> (feature.data);
                data = *data_ptr;
            } else {
                data = *static_cast<DataTypePtr> (feature.data);
                data_ptr = &data;
            }
        }
    }

protected:
    DataType data {};
    DataTypePtr data_ptr { nullptr };
};

}
