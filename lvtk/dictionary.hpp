
#pragma once

#include <lvtk/lvtk.hpp>
#include <lvtk/feature.hpp>

namespace lvtk {

class Dictionary
{
public:
    URID map (const String& uri) const {
        return p_map != nullptr ? p_map->map (p_map->handle, uri.c_str())
                                : 0;
    }

    String unmap (URID urid) const {
        return p_unmap != nullptr ? p_unmap->unmap (p_unmap->handle, urid)
                                  : String();
    }

    void set_feature (const Feature& map)
    {
        if (strcmp (map.URI, LV2_URID__map) == 0)
            p_map = (LV2_URID_Map*) map.data;
        else if (strcmp (map.URI, LV2_URID__unmap) == 0)
            p_unmap = (LV2_URID_Unmap*) map.data;
    }

private:
    LV2_URID_Map* p_map = nullptr;
    LV2_URID_Unmap* p_unmap = nullptr;    
};

}
