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
#include <string>

#include <lv2/core/lv2.h>
#include <lv2/urid/urid.h>
#include <lv2/worker/worker.h>

namespace lvtk {

using Handle        = LV2_Handle;
using URID          = LV2_URID;
using ExtensionMap  = std::map<std::string, const void*>;

template<class D>
struct DescriptorList final : public std::vector<D>
{
    inline ~DescriptorList() {
        for (const auto& desc : *this)
            std::free ((void*) desc.URI);
    }
};

struct Feature : LV2_Feature {
    Feature() = default;

    Feature (const LV2_Feature& feature)
    {
        data = feature.data;
        URI  = feature.URI;
    }

    inline bool operator== (const char* uri)  const { return strcmp (uri, URI) == 0; }
    inline bool operator== (const std::string& uri) const { return strcmp (uri.c_str(), URI) == 0; }
};

/** Convenient typedef for a vector of Features. */
struct FeatureList final : public std::vector<Feature>
{
    FeatureList() = default;
    FeatureList (const FeatureList& o) {
        for (const auto& f : o)
            push_back (f);
    }

    FeatureList (const LV2_Feature *const * features) {
        for (int i = 0; features[i]; ++i) {
            push_back (*features[i]);
        }
    }

    inline void get_uris (std::vector<std::string>& uris) const {
        for (const auto& f : *this)
            uris.push_back (f.URI);
    }
};

class FeatureIterator final
{
public:
    FeatureIterator (const Feature* const* features)
        : m_index (0), p_feats (features) { }

    inline const Feature* next()
    {
        if (nullptr == p_feats[m_index])
            return nullptr;
        return p_feats[m_index++];
    }

private:
    uint32_t                 m_index;
    const Feature* const*    p_feats;
};

struct InstanceArgs {
    InstanceArgs (const std::string& p, const std::string& b, const FeatureList& f)
        : plugin(p), bundle(b), features (f) {}

    std::string plugin;
    std::string bundle;
    FeatureList features;
};

struct Extension  {
    Extension (const std::string& _uri) 
        : URI (_uri) { }
    virtual ~Extension() = default;
    const std::string URI;

    inline bool operator== (const char* str_uri) const { return URI == str_uri; }
    inline bool operator!= (const char* str_uri) const { return URI != str_uri; }
};

/** Template class which can be used to assign feature data in a common way.
    Typically these are used to facilitate features provided by the host 
    during plugin instantiation or in host-side callbacks to the plugin.
 */
template<class D, bool StorePtr = false>
struct StackExtension : Extension
{
    using data_ptr_t = D*;

    StackExtension() = delete;

    explicit StackExtension (const char* feature_uri)
        : Extension (feature_uri) { }

    ~StackExtension() = default;

    inline bool is_for (const Feature& feature) const { return feature == URI; }

    inline void set_feature (const Feature& feature) {
        if (is_for (feature)) {
            if (StorePtr) {
                data_ptr = static_cast<data_ptr_t> (feature.data);
                data = *data_ptr;
            } else {
                data = *static_cast<data_ptr_t> (feature.data);
                data_ptr = &data;
            }
        }
    }

protected:
    D data { };
    data_ptr_t data_ptr { nullptr };
};

}
