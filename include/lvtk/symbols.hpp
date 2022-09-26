// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lv2/core/lv2.h>
#include <lv2/urid/urid.h>
#include <string>
#include <unordered_map>

namespace lvtk {

/** Maintains a map of Strings/Symbols to integers
    
    This class also implements LV2 URID Map/Unmap features.  Plugin 
    implementations don't need to use this.  You can, however, use this in a 
    LV2 host to easily provide URID map/unmaping features to plugins.

    @headerfile lvtk/symbols.hpp
    @ingroup lvtk
 */
class Symbols final {
public:
    /** Create an empty symbol map and initialized LV2 URID features */
    Symbols() {
        _mapd.handle   = (void*) this;
        _mapd.map      = _map;
        _unmapd.handle = (void*) this;
        _unmapd.unmap  = _unmap;
        refer_to (nullptr, nullptr);
    }

    ~Symbols() {
        refer_to (nullptr, nullptr);
        clear();
    }

    /** Map a symbol/uri to an unsigned integer
        @param key The symbol to map
        @returns A mapped URID, a return of 0 indicates failure */
    inline uint32_t map (const char* key) {
        if (! owner())
            return _mapref->map (_mapref->handle, key);

        if (! contains (key)) {
            const uint32_t urid (1 + (uint32_t) _mapped.size());
            _mapped[key]    = urid;
            _unmapped[urid] = std::string (key);
            return urid;
        }

        return _mapped[key];
    }

    /** Unmap an already mapped id to its symbol
        
        @param urid The URID to unmap
        @return The previously mapped symbol or 0 if the urid isn't in the cache
     */
    inline const char* unmap (uint32_t urid) {
        if (! owner())
            return _unmapref->unmap (_unmapref->handle, urid);
        if (contains (urid))
            return (const char*) _unmapped[urid].c_str();
        return "";
    }

    /** Containment test of a URI
        
        @param uri The URI to test
        @returns True if found. */
    inline bool contains (const char* uri) {
        return _mapped.find (uri) != _mapped.end();
    }

    /** Containment test of a URID
        
        @param urid The URID to test
        @return True if found */
    inline bool contains (uint32_t urid) {
        return _unmapped.find (urid) != _unmapped.end();
    }

    /** Clear the Symbols
        Does nothing if is refering to an external map/unmap.
     */
    inline void clear() {
        if (! owner())
            return;
        _mapped.clear();
        _unmapped.clear();
    }

    /** Returns true if this is Symbols owns the map */
    bool owner() const noexcept {
        return _mapref == nullptr || _unmapref == nullptr;
    }

    /** Refer this Symbols to raw Map/Unmap features */
    void refer_to (LV2_URID_Map* m, LV2_URID_Unmap* um) noexcept {
        _mapf.URI   = LV2_URID__map;
        _unmapf.URI = LV2_URID__unmap;

        if (m == nullptr || um == nullptr) {
            _mapf.data   = (void*) &_mapd;
            _unmapf.data = (void*) &_unmapd;
            _mapref      = nullptr;
            _unmapref    = nullptr;
            return;
        }

        _mapf.data = _mapref = m;
        _unmapf.data = _unmapref = um;
    }

    void refer_to (Symbols& o) noexcept {
        refer_to ((LV2_URID_Map*) o._mapf.data,
                  (LV2_URID_Unmap*) o._unmapf.data);
    }

    /** @returns a LV2_Feature with LV2_URID_Map as the data member */
    const LV2_Feature* const map_feature() const { return &_mapf; }
    /** @returns a LV2_Feature with LV2_URID_Unmap as the data member */
    const LV2_Feature* const unmap_feature() const { return &_unmapf; }

private:
    std::unordered_map<std::string, uint32_t> _mapped;
    std::unordered_map<uint32_t, std::string> _unmapped;

    LV2_Feature _mapf;
    LV2_URID_Map _mapd;
    LV2_URID_Map* _mapref { nullptr };

    LV2_Feature _unmapf;
    LV2_URID_Unmap _unmapd;
    LV2_URID_Unmap* _unmapref { nullptr };

    static uint32_t _map (LV2_URID_Map_Handle self, const char* uri) {
        return (static_cast<Symbols*> (self))->map (uri);
    }

    static const char* _unmap (LV2_URID_Unmap_Handle self, uint32_t urid) {
        return (static_cast<Symbols*> (self))->unmap (urid);
    }
};

} // namespace lvtk
