/**
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
*/
/**
   @file midi.hpp
   C++ convenience header for the LV2 midi extension.
*/

#ifndef LVTK_LV2_MIDI_HPP
#define LVTK_LV2_MIDI_HPP

#include <lv2/lv2plug.in/ns/ext/midi/midi.h>

#include <lvtk/types.hpp>

namespace lvtk {

   /** A mixin that allows easy sending of MIDI from GUI to plugin.

       @class lvtk::WriteMIDI
       @ingroup guimixins
   */
    LVTK_MIXIN_CLASS WriteMIDI {

     enum {
       EVENT_BUFFER_SIZE = 4
     };

     LVTK_MIXIN_DERIVED {

        I() : m_midi_type(0) {
         m_buffer = lv2_event_buffer_new(sizeof(LV2_Event) + EVENT_BUFFER_SIZE,
                                         0);
       }

       /** @internal */
       bool check_ok() {
         Derived* d = static_cast<Derived*>(this);
         m_midi_type = d->
           uri_to_id(LV2_EVENT_URI, "http://lv2plug.in/ns/ext/midi#MidiEvent");
         m_event_buffer_format = d->
           uri_to_id(LV2_UI_URI, "http://lv2plug.in/ns/extensions/ui#Events");
         if (LVTK_DEBUG) {
           if (m_midi_type != 0) {
             std::clog<<"    [LV2::WriteMIDI] Host does not map (\""
                      <<LV2_EVENT_URI
                      <<"\", \"http://lv2plug.in/ns/ext/midi#MidiEvent\") "
                      <<"to any valid ID.\n";
           }
           if (m_event_buffer_format != 0) {
             std::clog<<"    [LV2::WriteMIDI] Host does not map (\""
                      <<LV2_UI_URI
                      <<"\", \"http://lv2plug.in/ns/extensions/ui#Events\") "
                      <<"to any valid ID.\n";
           }
           if (!Required || (m_midi_type && m_event_buffer_format))
             std::clog<<"    [LV2::WriteMIDI] Validation succeeded."<<std::endl;
           else
             std::clog<<"    [LV2::WriteMIDI] Validation failed."<<std::endl;
         }
         return !Required || (m_midi_type && m_event_buffer_format);
       }

     protected:

       /** This function can be used to write a MIDI event to an event input port
           in the plugin.
           @param port The port index.
           @param size The number of bytes in the MIDI event.
           @param data The MIDI data for the event. This should point to an array
                       of @c size bytes.
           @return @c true if the event could be written to the plugin port,
                   @c false if it couldn't (for example if the host doesn't
                      support MIDI events).
       */
       bool write_midi(uint32_t port, uint32_t size, const uint8_t* data) {

         if (LVTK_DEBUG) {
           std::clog<<"[LV2::WriteMIDI] write_midi("<<port<<", "<<size
                    <<", \"";
           for (uint32_t i = 0; i < size; ++i) {
             if (i != 0)
               std::clog<<' ';
             std::clog<<std::hex<<std::setw(2)<<std::setfill('0')
                      <<static_cast<unsigned>(data[i]);
           }
           std::clog<<"\") -> ";
         }

         if (m_midi_type == 0) {
           if (LVTK_DEBUG)
             std::clog<<"false (Host does not support MIDI writing)"<<std::endl;
           return false;
         }

         LV2_Event_Buffer* buffer;
         if (size <= 4)
           buffer = m_buffer;
         else
           buffer = lv2_event_buffer_new(sizeof(LV2_Event) + size, 0);
         lv2_event_buffer_reset(m_buffer, 0, m_buffer->data);
         LV2_Event_Iterator iter;
         lv2_event_begin(&iter, m_buffer);
         lv2_event_write(&iter, 0, 0, m_midi_type, size, data);
         static_cast<Derived*>(this)->
           write(port, m_buffer->header_size + m_buffer->capacity,
                 m_event_buffer_format, m_buffer);
         if (size > 4)
           std::free(buffer);

         if (LVTK_DEBUG)
           std::clog<<"true"<<std::endl;
         return true;
       }

       uint32_t m_midi_type;
       uint32_t m_event_buffer_format;
       LV2_Event_Buffer* m_buffer;

     };

   };

} /* namespace lvtk */

#endif /* LVTK_LV2_MIDI_HPP */
