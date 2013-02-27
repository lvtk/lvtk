/****************************************************************************

    atom.hpp - support file for writing LV2 plugins in C++

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

#ifndef LVTK_ATOM_HPP
#define LVTK_ATOM_HPP

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>

#include <lvtk/ext/midi.hpp>
#include <lvtk/ext/patch.hpp>

namespace lvtk {


   /** Typedef for an Atom Forge */
   typedef LV2_Atom_Forge_Frame AtomForgeFrame;
   typedef LV2_Atom_Event       AtomEvent;

   /** Function type for mapping symbols */
   typedef uint32_t     (*MapFunc)(const char* symbol);

   /** Function type for unmaping URIDs */
   typedef const char*  (*UnmapFunc)(uint32_t id);

   struct AtomObject
   {
       typedef LV2_Atom_Property_Body Property;

       typedef std::vector<LV2_Atom_Property_Body*> PropertyVec;


      AtomObject (const void* atom) : p_obj ((LV2_Atom_Object*) atom) { }
      AtomObject (const AtomObject& other) : p_obj (other.p_obj)   { }

      LV2_Atom_Object* cobj()           { return p_obj; }
      operator LV2_Atom_Object*()       { return p_obj; }

      LV2_Atom* atom()                  { return (LV2_Atom*) p_obj; }

      LV2_URID otype() const            { return p_obj->body.otype; }

      uint32_t id() const               { return p_obj->body.id; }

      inline void
      query (LV2_Atom_Object_Query* query)
      {
          lv2_atom_object_query (p_obj, query);
      }

      void print ()
      {
          LV2_ATOM_OBJECT_FOREACH (p_obj, iter)
        {
              std::cout<< "key: " << iter->key << std::endl;

        }

      }
      const LV2_Atom*
      get_property_body (uint32_t key)
      {
          const LV2_Atom* value = NULL;
          LV2_Atom_Object_Query q[] = {{ key, &value }, LV2_ATOM_OBJECT_QUERY_END };
          query (q);
          return value;
      }

   private:

      LV2_Atom_Object* p_obj;

   };

   struct Atom
   {
      Atom (const void* atom) : p_atom ((LV2_Atom*) atom)  { }

      /** Pad a size to 64 bits */
      inline static uint32_t
      pad_size (uint32_t size)
      {
          return lv2_atom_pad_size (size);
      }

      /** Determine if the Atom is null */
      inline bool
      is_null()
      {
          return lv2_atom_is_null (p_atom);
      }

      /** Get the Atom's body */
      inline void*
      body() const
      {
          return LV2_ATOM_BODY (p_atom);
      }

      /** Get the Atom's body as a float */
      inline float
      as_float() const
      {
          return ((LV2_Atom_Float*)p_atom)->body;
      }

      const AtomObject
      as_object() const {
          return AtomObject ((LV2_Atom_Object* ) p_atom);
      }

      /** Get the Atom's body as a float */
      inline int
      as_int() const
      {
          return ((LV2_Atom_Int*)p_atom)->body;
      }

      /** Get this Atom's type */
      LV2_URID type() const
      {
          return p_atom->type;
      }

      /** Get the Atom's total size */
      inline uint32_t
      total_size() const
      {
          return lv2_atom_total_size (p_atom);
      }


      /** Get the Atom's body size */
      inline uint32_t
      size() const
      {
          return p_atom->size;
      }

      /** Get the underlying LV2_Atom pointer */
      inline const LV2_Atom*
      cobj() const
      {
          return p_atom;
      }

      inline operator const LV2_Atom*() { return cobj(); }

      inline Atom&
      operator= (const Atom& other)
      {
         p_atom = other.p_atom;
         return *this;
      }

      inline bool
      operator== (Atom& other)
      {
          return lv2_atom_equals (cobj(), other.cobj());
      }

   private:

      const LV2_Atom* p_atom;

   };


   /** Class wrapper around LV2_Atom_Forge */
   class AtomForge
   {
      LV2_Atom_Forge forge;
      LV2_URID midi_MidiEvent, patch_Set, patch_Get, patch_body;
      LV2_URID patch_subject;

   public:

      typedef LV2_Atom_Forge_Ref   Ref;
      typedef LV2_Atom_Forge_Frame Frame;

      /** Uninitialized AtomForge.
          @note Client code must call AtomForge::init() before using
       */
      AtomForge()
          : midi_MidiEvent(0), patch_Set(0),
            patch_Get(0), patch_body(0), patch_subject(0) { }

      /** Initialized AtomForge. */
      AtomForge (LV2_URID_Map* map)
      {
         init (map);
      }

      /** Initialize the underlying atom forge
          @param map The mapping function needed for init
       */
      inline void
      init (LV2_URID_Map* map)
      {
         lv2_atom_forge_init (&forge, map);
         midi_MidiEvent   = map->map (map->handle, LV2_MIDI__MidiEvent);
         patch_Set        = map->map (map->handle, LV2_PATCH__Set);
         patch_Get        = map->map (map->handle, LV2_PATCH__Get);
         patch_body       = map->map (map->handle, LV2_PATCH__body);
         patch_subject    = map->map (map->handle, LV2_PATCH__subject);
      }


      /** Get the underlying atom forge
          @return The forge
       */
      inline LV2_Atom_Forge*
      cobj()
      {
         return &forge;
      }

      /** Set the forge's buffer

          @param buf The buffer to use
          @param size The size of the buffer
       */
      inline void
      set_buffer (uint8_t* buf, uint32_t size)
      {
         lv2_atom_forge_set_buffer (&forge, buf, size);
      }

      /** Forge a simple MIDI note-on event

          @param key The midi key
          @param velocity The note's velocity
          @return An atom
       */
      inline LV2_Atom_Forge_Ref
      note_on (uint8_t key, uint8_t velocity, uint8_t chan = 0x0)
      {
         if (chan > 0x0F) chan = 0x0F;
         uint8_t midi[3] = { 0x90 | chan, key, velocity };
       //  Atom atom (lv2_atom_forge_atom (&forge, 3, midi_MidiEvent));
         return lv2_atom_forge_raw (&forge, midi, 3);
      }

      /**
       * Forge a MIDI note-off event
       * @param key The midi key
       * @return An atom
       */
      inline const LV2_Atom*
      note_off (uint8_t key)
      {
         uint8_t midi[3] = { 0x80, key, 0x00 };
         LV2_Atom* atom = (LV2_Atom*) lv2_atom_forge_atom (&forge, 3, midi_MidiEvent);
         lv2_atom_forge_raw (&forge, midi, 3);
         return atom;
      }

      /**
       * Forge an atom path from string
       * @param path The path to forge
       * @return An Atom
       */
      inline LV2_Atom_Forge_Ref
      path (const std::string& path)
      {
         return lv2_atom_forge_path (&forge, path.c_str(), path.size());
      }

      /** Forge an atom resource
          @return A reference to the Atom
       */
      inline LV2_Atom_Forge_Ref
      resource (AtomForgeFrame& frame, uint32_t id, uint32_t otype)
      {
         // Write object header
         return lv2_atom_forge_resource (&forge, &frame, id, otype);
      }

      inline LV2_Atom_Forge_Ref
      blank (AtomForgeFrame& frame, uint32_t id, uint32_t otype)
      {
         // Write object header
         return lv2_atom_forge_blank (&forge, &frame, id, otype);
      }

      inline LV2_Atom_Forge_Ref
      property_head (uint32_t key, uint32_t context)
      {
         return lv2_atom_forge_property_head (&forge, key, context);
      }

      inline LV2_Atom_Forge_Ref
      patch_get (LV2_URID subject, uint32_t id = 0, uint32_t context = 0)
      {
          AtomForgeFrame frame;
          LV2_Atom* atom = (LV2_Atom*) blank (frame, id, patch_Get);
          property_head (patch_subject, context);
          urid (subject);
          pop (frame);

          return (LV2_Atom_Forge_Ref) atom;
      }

      inline void
      pop (AtomForgeFrame& frame)
      {
         lv2_atom_forge_pop (&forge, &frame);
      }

      inline LV2_Atom_Forge_Ref
      integer (const int val)
      {
         return lv2_atom_forge_int (&forge, val);
      }

      inline LV2_Atom_Forge_Ref
      raw (const void* data, uint32_t size)
      {
         return lv2_atom_forge_raw (&forge, data, size);
      }

      inline LV2_Atom_Forge_Ref
      urid (LV2_URID id)
      {
          return lv2_atom_forge_urid (&forge, id);
      }

   };
} /* namespace lvtk */

#endif /* LVTK_ATOM_HPP */
