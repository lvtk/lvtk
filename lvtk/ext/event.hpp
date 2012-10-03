/**
    event.hpp - Support file for writing LV2 plugins in C++

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
   @file event.hpp
   C++ convenience header for the LV2 Event extension.
   LV2 C Version Support: 1.6 (2012-04-17) DEPRECATED

   This extension defines a generic time-stamped event port type, which can
   be used to create plugins that read and write real-time events, such as
   MIDI, OSC, or any other type of event payload. The type(s) of event supported
   by a port is defined in the data file for a plugin.
*/

#ifndef LVTK_LV2_EVENT_HPP
#define LVTK_LV2_EVENT_HPP

#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/event/event-helpers.h>

#warning "this header uses the deprecated lv2 extension event. \
	please use Atoms instead."

#include "lvtk/private/types.hpp"

namespace lvtk {

   /** The event ref/unref function, required for
       plugins with event ports.

       @ingroup pluginmixins
       @ingroup guimixins
       @headerfile lvtk/ext/event.hpp
       @deprecated
       @see The internal struct I for details
   */
   template <bool Required = true>
   struct EventRef
   {
	   template <class Derived>
	   struct I : Extension<Required>
	   {

         typedef I<Derived> Mixin;

         I() : m_callback_data(0), m_ref_func(0), m_unref_func(0) { }

         /** @internal */
         static void
         map_feature_handlers(feature_handler_map& hmap)
         {
            hmap[LV2_EVENT_URI] = &Mixin::handle_feature;
         }

         /** @internal */
         static void
         handle_feature(void* instance, void* data)
         {
            Derived* derived = reinterpret_cast<Derived*>(instance);
            I<Derived>* mixin = static_cast<I<Derived>*>(derived);

            LV2_Event_Feature* edata =
                        reinterpret_cast<LV2_Event_Feature*>(data);

            mixin->m_callback_data = edata->callback_data;
            mixin->m_ref_func      = edata->lv2_event_ref;
            mixin->m_unref_func    = edata->lv2_event_unref;

            mixin->m_ok = true;
         }

       bool
       check_ok() {
         if (LVTK_DEBUG) {
           std::clog<<"    [LV2::EventRef] Validation "
                    <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
         }
         return this->m_ok;
       }

     protected:

       /** This should be called by the plugin for any event of type 0 if it
           creates an additional copy of it, either by saving more than one copy
           internally, passing more than one copy through to an output port,
           or a combination of those. It must be called once for each additional
           copy of the event.
           Note that you must not call this function if you just save one copy
           of the event, or just passes one copy through to an output port.
           @c param event The event, as returned by @c lv2_event_get().
       */
       uint32_t
       event_ref(LV2_Event* event) {
         return m_ref_func(m_callback_data, event);
       }

       /** This should be called by the plugin for any event of type 0, unless
           it keeps a copy of it or passes it through to an event output port.
           @c param event The event, as returned by @c lv2_event_get().
       */
       uint32_t
       event_unref(LV2_Event* event) {
         return m_unref_func(m_callback_data, event);
       }

       LV2_Event_Callback_Data m_callback_data;
       uint32_t (*m_ref_func)(LV2_Event_Callback_Data, LV2_Event*);
       uint32_t (*m_unref_func)(LV2_Event_Callback_Data, LV2_Event*);

     };

   };

} /* namespace lvtk */

#endif /* LVTK_LV2_EVENT_HPP */
