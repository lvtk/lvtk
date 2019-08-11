
#pragma once

#include <map>
#include <string>
#include <lv2/core/lv2.h>
#include <lv2/urid/urid.h>

namespace lvtk {

/** Maintains a map of Strings/Symbols to integers
    This class also implements LV2 URID Map/Unmap features 
 */
class URIDirectory
{
public:
    /** Create an empty symbol map and initialized LV2 URID features */
    URIDirectory()
    {
        map_feature.URI    = LV2_URID__map;
        map_data.handle    = (void*) this;
        map_data.map       = &URIDirectory::_map;
        map_feature.data   = &map_data;

        unmap_feature.URI  = LV2_URID__unmap;
        unmap_data.handle  = this;
        unmap_data.unmap   = _unmap;
        unmap_feature.data = &unmap_data;
    }

    ~URIDirectory()
    {
        clear();
    }

    /** Map a symbol/uri to an unsigned integer
        @param key The symbol to map
        @return A mapped URID, a return of 0 indicates failure */
    inline uint32_t map (const char* key)
    {
        if (! contains (key))
        {
            const uint32_t urid (1 + (uint32_t) mapped.size());
            mapped [key] = urid;
            unmapped [urid] = std::string (key);
            return urid;
        }

        return mapped [key];
    }

    /** Containment test of a URI
        
        @param uri The URI to test
        @return True if found */
    inline bool contains (const char* uri) {
        return mapped.find (uri) != mapped.end();
    }

    /** Containment test of a URID
        
        @param urid The URID to test
        @return True if found */
    inline bool contains (uint32_t urid) {
        return unmapped.find (urid) != unmapped.end();
    }

    /** Unmap an already mapped id to its symbol
        @param urid The URID to unmap
        @return The previously mapped symbol or 0 if the urid isn't in the cache */
    inline const char* unmap (uint32_t urid) {
        if (contains (urid))
            return (const char*) unmapped [urid].c_str();
        return "";
    }

    /** Clear the URIDirectory */
    inline void clear()
    {
        mapped.clear();
        unmapped.clear();
    }

    const LV2_Feature *const get_map_feature()      const { return &map_feature; }
    const LV2_Feature *const get_unmap_feature()    const { return &unmap_feature; }

private:
    std::map<std::string, uint32_t> mapped;
    std::map<uint32_t, std::string> unmapped;

    LV2_Feature         map_feature;
    LV2_URID_Map        map_data;
    LV2_Feature         unmap_feature;
    LV2_URID_Unmap      unmap_data;

    static uint32_t _map (LV2_URID_Map_Handle self, const char* uri) {
        return (static_cast<URIDirectory*> (self))->map (uri);
    }

    static const char* _unmap (LV2_URID_Unmap_Handle self, uint32_t urid) {
        return (static_cast<URIDirectory*> (self))->unmap (urid);
    }
};

}
