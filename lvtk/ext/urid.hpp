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
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

namespace lvtk {

/** LV2_URID_Map wrapper
    @headerfile lvtk/ext/urid.hpp
 */
class Map : public FeatureData<LV2_URID_Map>
{
public:
    Map() : FeatureData<LV2_URID_Map> (LV2_URID__map) {}
    Map (const Feature& f) :FeatureData<LV2_URID_Map> (LV2_URID__map) {
        set_feature (f);
    }

    /** Get URID integer from URI string
        @param uri  The URI string to map
     */
    URID map (const std::string& uri) const {
        return data.map != nullptr ? data.map (data.handle, uri.c_str())
                                : 0;
    }

    URID operator()(const std::string& uri) const { return this->map (uri); }
};

/** LV2_URID_Unmap wrapper
    @headerfile lvtk/ext/urid.hpp
 */
class Unmap : public FeatureData<LV2_URID_Unmap>
{
public:
    Unmap() : FeatureData<LV2_URID_Unmap> (LV2_URID__unmap) {}
    Unmap (const Feature& feature) : FeatureData<LV2_URID_Unmap> (LV2_URID__unmap) {
        set_feature (feature);
    }

    /** Unmap a URID to string
        @param urid The URID integer to unmap
     */
    std::string unmap (URID urid) const {
        return data.unmap != nullptr ? data.unmap (data.handle, urid)
                                     : std::string();
    }

    /** @see unmap */
    std::string operator() (const URID urid) const { return this->unmap (urid); }
};

}
