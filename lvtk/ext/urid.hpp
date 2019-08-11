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

#include <lvtk/feature.hpp>
#include <lv2/urid/urid.h>

namespace lvtk {

class Map
{
public:
    Map() = default;
    Map (const Feature& f) {
        set_feature (f);
    }

    URID map (const std::string& uri) const {
        return p_map != nullptr ? p_map->map (p_map->handle, uri.c_str())
                                : 0;
    }

    URID operator()(const std::string& uri) const { return this->map (uri); }
    
    LV2_URID_Map* c_obj() const { return p_map; }

    void set_feature (const Feature& map) {
        if (strcmp (map.URI, LV2_URID__map) == 0)
            p_map = (LV2_URID_Map*) map.data;
    }

private:
    LV2_URID_Map* p_map = nullptr;
};

class Unmap
{
public:
    Unmap() = default;
    Unmap (const Feature& feature) {
        set_feature (feature);
    }

    std::string operator() (const URID urid) const { return this->unmap (urid); }

    std::string unmap (URID urid) const {
        return p_unmap != nullptr ? p_unmap->unmap (p_unmap->handle, urid)
                                  : std::string();
    }

    void set_feature (const Feature& map) {
        if (strcmp (map.URI, LV2_URID__unmap) == 0)
            p_unmap = (LV2_URID_Unmap*) map.data;
    }

private:
    LV2_URID_Unmap* p_unmap = nullptr;    
};

}
