// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/extension.hpp"

#include <lv2/urid/urid.h>

namespace lvtk {

/** LV2_URID_Map wrapper
    @headerfile lvtk/ext/urid.hpp
    @ingroup utility
 */
struct Map final : FeatureData<LV2_URID_Map> {
    Map() : FeatureData (LV2_URID__map) {}
    Map (const Feature& f) : FeatureData (LV2_URID__map) {
        set (f);
    }

    Map (LV2_URID_Map* map) : FeatureData (LV2_URID__map) {
        Feature f { LV2_URID__map, map };
        set (f);
    }

    /** Get URID integer from URI string
        @param uri  The URI string to map
     */
    uint32_t operator() (const std::string& uri) const {
        return data != nullptr ? data->map (data->handle, uri.c_str())
                               : 0;
    }
};

/** LV2_URID_Unmap wrapper
    @headerfile lvtk/ext/urid.hpp
    @ingroup utility
 */
struct Unmap final : FeatureData<LV2_URID_Unmap> {
    Unmap() : FeatureData (LV2_URID__unmap) {}
    Unmap (const Feature& feature) : FeatureData (LV2_URID__unmap) {
        set (feature);
    }

    /** Unmap a URID to string
        @param urid The URID integer to unmap
     */
    std::string operator() (const uint32_t urid) const {
        return data != nullptr ? data->unmap (data->handle, urid)
                               : std::string();
    }
};

/** Adds URID `map` and `unmap` to your instance
    @ingroup ext
    @headerfile lvtk/ext/urid.hpp
*/
template <class I>
struct URID : NullExtension {
    /** @private */
    URID (const FeatureList& features) {
        for (const auto& f : features) {
            if (! _map)
                _map.set (f);
            if (! _unmap)
                _unmap.set (f);
            if (_map && _unmap)
                break;
        }
    }

    /** Map a uri */
    uint32_t map_uri (const std::string& uri) const noexcept { return _map (uri); }

    /** Unmap a URID */
    std::string unmap_urid (uint32_t urid) const noexcept { return _unmap (urid); }

private:
    Map _map;
    Unmap _unmap;
};

} // namespace lvtk
