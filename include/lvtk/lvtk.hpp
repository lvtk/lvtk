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
    For both the @ref Plugin and the @ref UI classes there are other helper
    classes called @ref Extension "Extensions" that you can use to add extra
    functionality to your plugin or UI.

    <b>Versioning</b><br>
    This library is header only, thus ABI stability is not an issue. The API
    will be stable between major version bumps, at which the pkg-config name
    would change to prevent plugins from building against an incompatible
    version.

    @author Michael Fisher <mfisher@kushview.net>
    @example volume.cpp
 */

#pragma once

#include <lv2/core/lv2.h>
#include <lv2/urid/urid.h>

#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace lvtk {
/** @defgroup lvtk Core
    Misc classes and types
    @{
 */

/** Alias to LV2_Handle */
using Handle = LV2_Handle;

/** Map of extension data */
using ExtensionMap = std::map<std::string, const void*>;

/** Internal class which maintains a list of descriptors */
template <class D>
struct DescriptorList final : public std::vector<D> {
    inline ~DescriptorList() {
        for (const auto& desc : *this)
            std::free ((void*) desc.URI);
    }
};

/** C++ version of an LV2_Feature */
struct Feature : LV2_Feature {
    Feature() = default;

    Feature (const LV2_Feature& feature) {
        data = feature.data;
        URI = feature.URI;
    }

    /** @returns true if this Feature's URI matches */
    inline bool operator== (const char* uri) const { return strcmp (uri, URI) == 0; }

    /** @returns true if this Feature's URI matches */
    inline bool operator== (const std::string& uri) const { return strcmp (uri.c_str(), URI) == 0; }
};

/** A Vector of Features.

    This is used to prepare LV2_Feature arrays for use by instances
    and extensions during instantiation of Plugins and UIs.

    @note This contains external data from the host and should never
    itself be referenced by your plugin.
 */
struct FeatureList final : public std::vector<Feature> {
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
    FeatureList (const LV2_Feature* const* features) {
        for (int i = 0; features[i]; ++i) {
            push_back (*features[i]);
        }
    }

    inline void* data (const std::string& uri) const {
        for (const auto& f : *this)
            if (f == uri)
                return f.data;
        return nullptr;
    }

    /** Returns true if the uri is found */
    inline bool contains (const std::string& uri) const {
        return data (uri) != nullptr;
    }
};

/** Template class which can be used to assign feature data in a common way.

    Typically these are used to facilitate features provided by the host
    during plugin instantiation or in host-side callbacks to the plugin.

    @tparam D   The data type
    @tparam P   The data's pointer type. Usefull to set P = D
                when D itself is a pointer.

    @see @ref WorkerSchedule, @ref Map, @ref Unmap, @ref Logger
 */
template <class D, class P = D*>
struct FeatureData {
    using data_type = D;
    using data_ptr_type = P;

    /** A uri for this data */
    const std::string URI;

public:
    FeatureData() = delete;
    ~FeatureData() = default;

    /** Returns true if the passed feature is for this Feature Data
        @param feature  The feature to check
     */
    inline bool is_for (const Feature& feature) const noexcept { return feature == URI; }

    /** @returns a pointer to the underlying data_t */
    inline data_ptr_type get() const noexcept { return data; }

    /** Sets the data from a feature
        @param feature  The Feature to check and set
     */
    inline bool set (const Feature& feature) noexcept {
        if (! is_for (feature))
            return false;
        data = (data_ptr_type) feature.data;
        return true;
    }

    /** false if the data ptr is null */
    inline operator bool() const noexcept { return data != nullptr; }

    /** castable to `data_ptr_type` */
    inline operator data_ptr_type() const noexcept { return data; }

    /** Call as function
        @returns The data pointer
     */
    inline data_ptr_type operator()() const noexcept { return data; }

protected:
    /** Subclasses must use this ctor to assign the URI of the data */
    explicit FeatureData (const char* feature_uri)
        : URI (feature_uri) {}

    data_ptr_type data = nullptr;
};
/* @} */
} // namespace lvtk
