/****************************************************************************

    urid.hpp - support file for writing LV2 plugins in C++

    Copyright (C) 2012 Michael Fisher <mfisher31@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef LVTK_LV2_URID_HPP
#define LVTK_LV2_URID_HPP

#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

namespace lvtk {

   /**
      The URID Mixin.
      @headerfile lvtk/urid.hpp
      @class lvtk::URID
      @ingroup pluginmixins
   */
   LVTK_MIXIN_CLASS URID {
     LVTK_MIXIN_DERIVED {
     
         I() : p_unmap(NULL), p_map(NULL) { }

         /** @internal */
         static void
         map_feature_handlers(feature_handler_map& hmap)
         {
            hmap[LV2_URID__map] = &I<Derived>::handle_map_feature;
            hmap[LV2_URID__unmap] = &I<Derived>::handle_unmap_feature;
         }

         /** @internal */
         static void
         handle_map_feature(void* instance, void* data)
         {
            Derived* d = reinterpret_cast<Derived*>(instance);
            I<Derived>* mixin = static_cast<I<Derived>*>(d);

            mixin->p_map = reinterpret_cast<LV2_URID_Map*>(data);
            mixin->m_ok = true;
         }

         /** @internal */
         static void
         handle_unmap_feature(void* instance, void* data)
         {
            Derived* d = reinterpret_cast<Derived*>(instance);
            I<Derived>* mixin = static_cast<I<Derived>*>(d);

            mixin->p_unmap = reinterpret_cast<LV2_URID_Unmap*> (data);
            mixin->m_ok = true;
         }


         bool
         check_ok()
         {
            if (LVTK_DEBUG) {
              std::clog<<"    [LV2::URID] Validation "
                       <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
            }
            return this->m_ok;
         }

     protected:

         /**
            Get the URI for a previously mapped numeric ID.

            Returns NULL if @p urid is not yet mapped.  Otherwise, the corresponding
            URI is returned in a canonical form.  This MAY not be the exact same
            string that was originally passed to LV2_URID_Map::map(), but it MUST be
            an identical URI according to the URI syntax specification (RFC3986).  A
            non-NULL return for a given @p urid will always be the same for the life
            of the plugin.  Plugins that intend to perform string comparison on
            unmapped URIs SHOULD first canonicalise URI strings with a call to
            map_uri() followed by a call to unmap_uri().

            @param urid The ID to be mapped back to the URI string.
         */
         const char*
         unmap (LV2_URID urid)
         {
            return p_unmap->unmap (p_unmap->handle, urid);
         }

         /**
            Get the numeric ID of a URI.

            If the ID does not already exist, it will be created.

            This function is referentially transparent; any number of calls with the
            same arguments is guaranteed to return the same value over the life of a
            plugin instance.  Note, however, that several URIs MAY resolve to the
            same ID if the host considers those URIs equivalent.

            This function is not necessarily very fast or RT-safe: plugins SHOULD
            cache any IDs they might need in performance critical situations.

            The return value 0 is reserved and indicates that an ID for that URI
            could not be created for whatever reason.  However, hosts SHOULD NOT
            return 0 from this function in non-exceptional circumstances (i.e. the
            URI map SHOULD be dynamic).

            @param uri The URI to be mapped to an integer ID.
         */
         LV2_URID
         map (const char* uri)
         {
            return p_map->map (p_map->handle, uri);
         }

         LV2_URID_Map   *p_map;
         LV2_URID_Unmap *p_unmap;

     };
   };
}

#endif /* LVTK_LV2_URID_HPP */
