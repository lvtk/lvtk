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

/** @defgroup urid URID 
    Working with URIDs
*/

#pragma once

#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {

/** LV2_URID_Map wrapper
    @headerfile lvtk/ext/urid.hpp
    @ingroup urid
 */
class Map : public FeatureData<LV2_URID_Map>
{
public:
    Map() : FeatureData (LV2_URID__map) {}
    Map (const Feature& f) : FeatureData (LV2_URID__map) {
        set (f);
    }

    /** Get URID integer from URI string
        @param uri  The URI string to map
     */
    uint32_t operator()(const std::string& uri) const {
        return data != nullptr ? data->map (data->handle, uri.c_str())
                               : 0;
    }
};

/** LV2_URID_Unmap wrapper
    @headerfile lvtk/ext/urid.hpp
    @ingroup urid
 */
class Unmap : public FeatureData<LV2_URID_Unmap>
{
public:
    Unmap() : FeatureData (LV2_URID__unmap) {}
    Unmap (const Feature& feature) : FeatureData (LV2_URID__unmap) {
        set (feature);
    }

    /** Unmap a URID to string
        @param urid The URID integer to unmap
     */
    std::string operator()(const uint32_t urid) const {
        return data != nullptr ? data->unmap (data->handle, urid)
                               : std::string();
    }
};

/** Adds URID `map` and `unmap` to your instance
    @ingroup urid
    @headerfile lvtk/ext/urid.hpp
*/
template<class I> 
struct URID : NullExtension
{
    /** @private */
    URID (const FeatureList& features) {
        for (const auto& f : features) {
            if (! map)   map.set (f);
            if (! unmap) unmap.set (f);
            if (map && unmap)
                break;
        }
    }

protected:
    /** The Map is exposed as protected function object. You can call it as such. 
        e.g. `map ("http://someuri.com/#somekey");`
     */
    Map map;

    /** The Unmap is exposed as protected function object. You can call it as such. 
        e.g. `unmap (a_mapped_uri_int);`
     */
    Unmap unmap;
};

}
