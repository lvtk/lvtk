// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

/** @mainpage LV2 Toolkit

    @section intro Introduction
    These documents describe some C++ classes that may be of use if you want
    to write LV2 plugins in C++. They implement most of the boilerplate code
    so you only have to write the parts that matter, and hide the low-level
    C API.

    <b>Extension Mixins</b><br>
    For both the @ref lvtk::Plugin and the @ref lvtk::UI classes there are other helper
    classes called Extensions that you can use to add extra
    functionality to your plugin or UI.

    <b>Library Versioning</b><br>
    The core library is header only, thus ABI stability is not an issue. The API
    will be stable between major version bumps, at which the pkg-config name
    would change to prevent plugins from building against an incompatible
    version.

    <b>ABI Versioning</b><br>
    The widgets and host libraries is not header only, thus ABI stability is
    a concern when adding new code. ABI version number will bump when compatibility
    breaks.

    @author Michael Fisher <mfisher@lvtk.org>
    @example volume.cpp
    @example volume-ui.cpp
 */

#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include <lv2/core/lv2.h>
#include <lvtk/lvtk.h>

namespace lvtk {
/** @defgroup alias Alias 
    Aliases to LV2 C-types 
 */

/** @defgroup utility Utility
    Utility classes and functions
 */

/** @defgroup wrapper Wrapper
    Slim wrappers around LV2 types.
 */

/** @defgroup graphics Graphics
    A graphics API
*/

/** @defgroup lvtk Core
    Core classes, types, and utilities.
    @{
 */

/** Alias to LV2_Handle 
    @ingroup lvtk
*/
using Handle = LV2_Handle;

/** Map of extension data
    @ingroup lvtk
*/
using ExtensionMap = std::map<std::string, const void*>;

/** Internal class which maintains a list of descriptors
    @ingroup lvtk
*/
template <class D>
struct DescriptorList final : public std::vector<D> {
    inline ~DescriptorList() {
        for (const auto& desc : *this)
            std::free ((void*) desc.URI);
    }
};

/** C++ version of an LV2_Feature 
    @headerfile lvtk/lvtk.hpp
    @ingroup lvtk
*/
struct Feature : LV2_Feature {
    /** Blank feature */
    Feature() = default;

    /** Reference another feature 
        @param feature
    */
    Feature (const LV2_Feature& feature) {
        data = feature.data;
        URI  = feature.URI;
    }

    /** Reference another feature
        @param feature_uri
        @param feature_data
    */
    Feature (const char* feature_uri, void* feature_data) {
        data = feature_data;
        URI  = feature_uri;
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

    @headerfile lvtk/lvtk.hpp
    @ingroup lvtk
 */
class FeatureList final : public std::vector<Feature> {
public:
    /** Construct an empty feature list */
    FeatureList() = default;

    /** Copy features. Data pointers are referenced
        @param o The other list to add from.
     */
    FeatureList (const FeatureList& o) {
        for (const auto& f : o)
            push_back (f);
    }

    /** Contstruct from raw LV2_Feature array
        @param features  LV2_Feature array to reference
    */
    FeatureList (const LV2_Feature* const* features) {
        add (features);
    }

    /** Contstruct from a LV2_Feature vector.
        @param features  LV2_Feature array to reference
    */
    FeatureList (const std::vector<const LV2_Feature*>& features) {
        add (features);
    }

    inline void add (const LV2_Feature* const* features) noexcept {
        for (int i = 0; features[i] != nullptr; ++i)
            push_back (*features[i]);
    }

    /** Add some features to the list. 
     
        @param features A vector of features to add.
    */
    inline void add (const std::vector<const LV2_Feature*>& features) noexcept {
        for (const auto* f : features)
            if (nullptr != f)
                push_back (*f);
    }

    /** Returns The data associated with a contained feature.

        @param uri The URI of the feature to look for
        @returns The data ptr or nullptr if not found
     */
    inline void* data (const std::string& uri) const noexcept {
        auto iter = std::find_if (begin(), end(), [&uri] (const Feature& f) -> bool {
            return strcmp (uri.c_str(), f.URI) == 0;
        });
        return iter != end() ? iter->data : nullptr;
    }

    /** @returns true if the uri is found */
    inline bool contains (const std::string& uri) const noexcept {
        return data (uri) != nullptr;
    }

    /** Returns a null-terminated version of these Features. 
     
        The return value is ONLY valid as long as this list hasn't been deleted,
        or the contents modified by adding and removing features.
    */
    auto null_terminated() const noexcept {
        if (_raw.size() != size() + 1 || (! _raw.empty() && _raw.back() != nullptr)) {
            _raw.clear();
            _raw.reserve (size() + 1);
            for (const auto& f : *this)
                _raw.push_back (&f);
            _raw.push_back (nullptr);
        }
        return (const LV2_Feature* const*) _raw.data();
    }

    /** Pass to functions accepting `const LV2_Feature* const*` as a parameter. */
    inline operator const LV2_Feature* const*() const noexcept {
        return null_terminated();
    }

private:
    mutable std::vector<const LV2_Feature*> _raw;
};

/** Template class which can be used to assign feature data in a common way.

    Typically these are used to facilitate features provided by the host
    during plugin instantiation or in host-side callbacks to the plugin.

    @tparam D   The data type
    @tparam P   The data's pointer type. Usefull to set P = D
                when D itself is a pointer.

    @see @ref WorkerSchedule, @ref Map, @ref Unmap, @ref Logger

    @ingroup lvtk
    @headerfile lvtk/lvtk.hpp
 */
template <class D, class P = D*>
struct FeatureData {
    using data_type     = D;
    using data_ptr_type = P;

    /** A uri for this data */
    const std::string URI;

public:
    FeatureData()  = delete;
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

    inline bool set (const Feature* const feature) noexcept {
        return set (*feature);
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
