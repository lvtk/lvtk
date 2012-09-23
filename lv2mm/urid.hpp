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

#ifndef LV2_URID_HPP
#define LV2_URID_HPP

#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

namespace LV2 {

  /**
   * The URID Extension.
   * The actual type that your plugin class will inherit when you use
   * this mixin is the internal struct template I.
   * @ingroup pluginmixins
   */
   template <bool Required = true>
   struct URID {

     /** This is the type that your plugin class will inherit when you use the
         EventRef mixin. The public and protected members defined here
         will be available in your plugin class.
     */
     template <class Derived> struct I : Extension<Required> {

       /** @internal */
       I() { }

       /** @internal */
       static void map_feature_handlers(FeatureHandlerMap& hmap) {
         hmap[LV2_URID__map] = &I<Derived>::handle_map_feature;
         hmap[LV2_URID__unmap] = &I<Derived>::handle_unmap_feature;
       }

       /** @internal */
       static void handle_map_feature(void* instance, void* data) {
         Derived* d = reinterpret_cast<Derived*>(instance);
         I<Derived>* fe = static_cast<I<Derived>*>(d);

         fe->p_map = reinterpret_cast<LV2_URID_Map*>(data);

         fe->m_ok = true;
       }

       /** @internal */
       static void handle_unmap_feature(void* instance, void* data) {
         Derived* d = reinterpret_cast<Derived*>(instance);
         I<Derived>* fe = static_cast<I<Derived>*>(d);

         fe->p_unmap = reinterpret_cast<LV2_URID_Unmap*> (data);

         fe->m_ok = true;
       }


       bool check_ok() {
         if (LV2CXX_DEBUG) {
           std::clog<<"    [LV2::URID] Validation "
                    <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
         }
         return this->m_ok;
       }


     protected:

       const char* unmap (LV2_URID urid)
           { return p_unmap->unmap (p_unmap->handle, urid); }

       LV2_URID  map (const char* uri)
           { return p_map->map (p_map->handle, uri); }

       LV2_URID_Map   *p_map;
       LV2_URID_Unmap *p_unmap;

     };
   };
}

#endif /* LV2_URID_HPP */
