/**
    urimap.hpp - Support file for writing LV2 plugins in C++

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
*/
/**
   @file urimap.hpp
   C++ convenience header for the LV2 URIMap extension.
   LV2 C Version: 1.6 (2012-04-17) DEPRECATED

   This extension is deprecated. New implementations should use LV2 URID instead.

   This extension defines a simple mechanism for plugins to map URIs to integers,
   usually for performance reasons (e.g. processing events typed by URIs in
   real time). The expected use case is for plugins to map URIs to integers
   for things they 'understand' at instantiation time, and store those values
   for use in the audio thread without doing any string comparison. This allows
   the extensibility of RDF with the performance of integers (or centrally defined
   enumerations).
*/

#ifndef DAPS_LV2_URIMAP_HPP
#define DAPS_LV2_URIMAP_HPP

#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>
#include <daps/types.hpp>

namespace daps {

   /** The URI map mixin. This can be used by both plugins and GUIs.

       The actual type that your plugin class will inherit when you use
       this mixin is the internal struct template I.
       @ingroup pluginmixins
       @ingroup guimixins
   */
   DAPS_MIXIN_CLASS URIMap {
     DAPS_MIXIN_DERIVED {

        I() : m_callback_data(0), m_func(0) { }

       /** @internal */
       static void
       map_feature_handlers(feature_handler_map& hmap) {
         hmap[DAPS_URI_MAP_URI] = &I<Derived>::handle_feature;
       }

       /** @internal */
       static void
       handle_feature(void* instance, void* data) {
         Derived* d = reinterpret_cast<Derived*>(instance);
         I<Derived>* fe = static_cast<I<Derived>*>(d);
         DAPS_URI_Map_Feature* umf = reinterpret_cast<DAPS_URI_Map_Feature*>(data);
         fe->m_callback_data = umf->callback_data;
         fe->m_func = umf->uri_to_id;
         fe->m_ok = (fe->m_func != 0);
       }

       bool check_ok() {
         if (DAPS_DEBUG) {
           std::clog<<"    [LV2::URIMap] Validation "
                    <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
         }
         return this->m_ok;
       }

     protected:

       /** This returns a numeric identifier for a given URI and map.
           This is used for e.g. getting numeric IDs for event types
           specified by URIs. A return value of 0 should be considered to
           mean that the URI you passed is not supported by the host.
           @param map An URI to be used as namespace.
           @param uri The URI that you want to map to a numeric ID.
       */
       uint32_t
       uri_to_id(const char* map, const char* uri) const {
         if (DAPS_DEBUG) {
           uint32_t result = m_func(m_callback_data, map, uri);
           std::clog<<"[LV2::URIMap] uri_to_id(\""<<uri<<"\") -> "
                    <<result<<std::endl;
           return result;
         }
         return m_func(m_callback_data, map, uri);
       }

       DAPS_URI_Map_Callback_Data m_callback_data;
       uint32_t (*m_func)(DAPS_URI_Map_Callback_Data, const char*, const char*);

     };

   };

} /* namespace daps */

#endif /* DAPS_LV2_URIMAP_HPP */
