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

/** @mainpage LV2 Toolkit
    
    @section intro Introduction
    These documents describe some C++ classes that may be of use if you want
    to write LV2 plugins in C++. They implement most of the boilerplate code
    so you only have to write the parts that matter, and hide the low-level
    C API.

    <b>Extension Mixins</b><br>
    For both the Instance and the UIInstance classes there are other helper 
    classes called @ref Extension "Extensions" that you can use to add extra 
    functionality to your plugin or UI.
    
    <b>Versioning</b><br>
    This library is available as a header only, thus ABI stability is not an
    issue. The API will be stable between major version bumps, at which the
    pkg-config name would change to prevent plugins from building against an
    incompatible version.

    @author Michael Fisher <mfisher@kushview.net>
    @example volume.cpp
    @example volume_ui.cpp
 */

#pragma once

#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

namespace lvtk {
/** @defgroup lvtk Core
 
    Miscelaneous classes used by the rest of LVTK.
    @{
 */

/** Alias to LV2_Handle */
using Handle        = LV2_Handle;

/** Map of extension data */
using ExtensionMap  = std::map<std::string, const void*>;

/** Internal class which maintains a list of descriptors */
template<class D>
struct DescriptorList final : public std::vector<D>
{
    inline ~DescriptorList() {
        for (const auto& desc : *this)
            std::free ((void*) desc.URI);
    }
};

/** C++ version of an LV2_Feature */
struct Feature : LV2_Feature {
    Feature() = default;

    Feature (const LV2_Feature& feature)
    {
        data = feature.data;
        URI  = feature.URI;
    }

    /** @returns true if this Feature's URI matches */
    inline bool operator== (const char* uri)  const { return strcmp (uri, URI) == 0; }
    /** @returns true if this Feature's URI matches */
    inline bool operator== (const std::string& uri) const { return strcmp (uri.c_str(), URI) == 0; }
};

/** A Vector of Features.
    
    This is used to prepare LV2_Feature arrays for use by instances
    and extensions during instantiation of Plugins and UIs.

    @note This usually contains external data from the host and should never
    itself be referenced by your plugin
 */
struct FeatureList final : public std::vector<Feature>
{
    /** Construct an empty feature list */
    FeatureList() = default;

    /** Copy features. Data pointers are referenced */
    FeatureList (const FeatureList& o) {
        for (const auto& f : o)
            push_back (f);
    }

    /** Contstruct from raw LV2_Feature array
        
        @param feature  LV2_Feature array to reference
    */
    FeatureList (const LV2_Feature *const * features) {
        for (int i = 0; features[i]; ++i) {
            push_back (*features[i]);
        }
    }

    /** Populate a string vector with URIs contained in this list
        
        @param uris The vector to fill
    */
    inline void get_uris (std::vector<std::string>& uris) const {
        for (const auto& f : *this)
            uris.push_back (f.URI);
    }
};

/** Helper which makes iterating LV2_Features easy */
class FeatureIterator final
{
public:
    /** Iterate the given feature array
        @param features  The LV2_Feature array
     */
    FeatureIterator (const LV2_Feature* const* features)
        : index (0), feats (features) { }

    /** @returns the next feature in the list */
    inline const LV2_Feature* next() const
    {
        if (nullptr == feats [index])
            return nullptr;
        return feats [index++];
    }

private:
    mutable uint32_t            index = 0;
    const LV2_Feature* const*   feats = nullptr;
};

/** Arguments passed to a plugin instance */
struct Args
{
    Args() : sample_rate(0.0), bundle(), features() {}
    Args (double r, const std::string& b, const FeatureList& f)
        : sample_rate(r), bundle (b), features (f) { }

    double sample_rate;     ///< Sample Rate
    std::string bundle;     ///< Bundle Path
    FeatureList features;   ///< Host provided features

    void clear() {
        sample_rate = 0.0;
        bundle = {};
        features.clear();
    }
    
    /** Ensures args are initially clear, and also cleared
        when it has gone out of scope
     */
    struct Cleared final
    {
        Cleared (Args& a) : args (a) { args.clear(); }
        ~Cleared() { args.clear(); }
        Args& args;
    };
};

/** Template class which can be used to assign feature data in a common way.
    
    Typically these are used to facilitate features provided by the host 
    during plugin instantiation or in host-side callbacks to the plugin.

    @see Scheduler
 */
template<class D>
struct FeatureData
{
    using data_t        = D;
    using data_ptr_t    = D*;

    /** A uri for this data */
    const std::string URI;

protected:
    /** Subclasses must use this ctor to assing the URI of the data */
    explicit FeatureData (const char* feature_uri)
        : URI (feature_uri) { }

public:
    FeatureData() = delete;
    ~FeatureData() = default;

    /** Returns true if the passed feature is for this Feature Data
        @param feature  The feature to check
     */
    inline bool is_for (const Feature& feature) const { return feature == URI; }

    /** @returns a C-Type pointer to the underlying data_t */
    inline data_ptr_t c_obj() const { return (data_ptr_t) &data; }

    /** Sets the data from a feature
        @param feature  The Feature to check and set
     */
    inline bool set_feature (const Feature& feature) {
        if (is_for (feature)) {
            data = *static_cast<data_ptr_t> (feature.data);
            return true;
        }
        return false;
    }

protected:
    data_t data {};
};
/* @} */
}
