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

#ifndef LVTK_LV2_ATOM_HPP
#define LVTK_LV2_ATOM_HPP

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>

#include <lvtk/ext/midi.hpp>
#include <lvtk/ext/patch.hpp>

namespace lvtk {

   typedef LV2_URID URID;

   /** Typedef for an Atom Forge */
   typedef LV2_Atom_Forge_Frame AtomForgeFrame;

   /** Function type for mapping symbols */
   typedef uint32_t     (*MapFunc)(const char* symbol);

   /** Function type for unmaping URIDs */
   typedef const char*  (*UnmapFunc)(uint32_t id);


   struct Atom
   {
      Atom (LV2_Atom* atom) : p_atom(atom) { }
      inline static uint32_t pad_size(uint32_t size) { return lv2_atom_pad_size(size); }
      bool is_null() { return lv2_atom_is_null(p_atom); }

      bool operator ==(Atom& other) { return lv2_atom_equals(cobj(), other.cobj()); }
      uint32_t total_size() const { return lv2_atom_total_size(p_atom); }
      uint32_t size() const { return p_atom->size; }
      URID type() const { return p_atom->type; }
      LV2_Atom* cobj() { return p_atom; }

   private:
      LV2_Atom* p_atom;
   };


   struct AtomObject
   {
      AtomObject (const void* atom) : p_object((LV2_Atom_Object*)atom) {}

      LV2_Atom_Object* cobj() { return p_object; }

      LV2_Atom* atom() { return (LV2_Atom*)p_object; }

      URID otype() const { return p_object->body.otype; }

      uint32_t id() const { return p_object->body.id; }

   private:
      LV2_Atom_Object* p_object;
   };


   /**
    * Class wrapper around LV2_Atom_Forge
    */
   class AtomForge
   {
      LV2_Atom_Forge forge;
      LV2_URID midi_MidiEvent, patch_Set, patch_Get, patch_body;

   public:

      /**
       * Uninitialized AtomForge.
       * @note Client code must call AtomForge::init() before using
       */
      AtomForge()
      : midi_MidiEvent(0)
      , patch_Set(0), patch_Get(0), patch_body(0)
      { }

      /**
       * Initialized AtomForge.
       */
      AtomForge (MapFunc map)
      {
         init (map);
      }

      /**
       * Initialize the underlying atom forge
       * @param map The mapping function needed for init
       */
      inline void
      init (MapFunc map)
      {
         assert (map != NULL);

         midi_MidiEvent = map(LV2_MIDI__MidiEvent);
         patch_Set      = map(LV2_PATCH__Set);
         patch_Get      = map(LV2_PATCH__Get);

         lv2_atom_forge_set_buffer (&forge, NULL, 0);
         forge.Blank    = map(LV2_ATOM__Blank);
         forge.Bool     = map(LV2_ATOM__Bool);
         forge.Chunk    = map(LV2_ATOM__Chunk);
         forge.Double   = map(LV2_ATOM__Double);
         forge.Float    = map(LV2_ATOM__Float);
         forge.Int      = map(LV2_ATOM__Int);
         forge.Long     = map(LV2_ATOM__Long);
         forge.Literal  = map(LV2_ATOM__Literal);
         forge.Path     = map(LV2_ATOM__Path);
         forge.Property = map(LV2_ATOM__Property);
         forge.Resource = map(LV2_ATOM__Resource);
         forge.Sequence = map(LV2_ATOM__Sequence);
         forge.String   = map(LV2_ATOM__String);
         forge.Tuple    = map(LV2_ATOM__Tuple);
         forge.URI      = map(LV2_ATOM__URI);
         forge.URID     = map(LV2_ATOM__URID);
         forge.Vector   = map(LV2_ATOM__Vector);
      }


      /**
       * Get the underlying atom forge
       * @return The forge
       */
      inline LV2_Atom_Forge*
      cobj()
      {
         return &forge;
      }

      /**
       * Set the forge's buffer
       * @param buf The buffer to use
       * @param size The size of the buffer
       */
      inline void
      set_buffer(uint8_t* buf, uint32_t size)
      {
         lv2_atom_forge_set_buffer (&forge, buf, size);
      }

      /**
       * Get an atom's total size
       * @param atom The atom to inspect (must be valid)
       */
      inline uint32_t
      atom_total_size (const LV2_Atom* atom)
      {
         return lv2_atom_total_size (atom);
      }

      /* MIDI Related */

      /**
       * Forge a simple MIDI note-on event
       * @param key The midi key
       * @param velocity The note's velocity
       * @return An atom
       */
      inline const LV2_Atom*
      note_on (uint8_t key, uint8_t velocity)
      {
         uint8_t midi[3];
         midi[0] = 0x90;
         midi[1] = key;
         midi[2] = velocity;

         LV2_Atom* atom = (LV2_Atom*) lv2_atom_forge_atom (&forge, 3, midi_MidiEvent);
         lv2_atom_forge_raw (&forge, midi, 3);
         return atom;
      }

      /**
       * Forge a MIDI note-off event
       * @param key The midi key
       * @return An atom
       */
      inline const LV2_Atom*
      note_off (uint8_t key)
      {
         uint8_t midi[3];
         midi[0] = 0x80;
         midi[1] = key;
         midi[2] = 0x00;

         LV2_Atom* atom = (LV2_Atom*) lv2_atom_forge_atom (&forge, 3, midi_MidiEvent);
         lv2_atom_forge_raw (&forge, midi, 3);
         return atom;
      }

      /**
       * Forge an atom path from string
       * @param path The path to forge
       * @return A reference to the Atom
       */
      inline LV2_Atom_Forge_Ref
      path (const std::string& path)
      {
         return lv2_atom_forge_path(&forge,path.c_str(),path.size());
      }

      /**
       * Forge an atom resource
       * @return A reference to the Atom
       */
      inline LV2_Atom_Forge_Ref
      resource (AtomForgeFrame *frame, uint32_t id, uint32_t otype)
      {
         // Write object header
         return lv2_atom_forge_resource (&forge, frame, id, otype);
      }

      inline LV2_Atom_Forge_Ref
      blank (AtomForgeFrame *frame, uint32_t id, uint32_t otype)
      {
         // Write object header
         return lv2_atom_forge_blank (&forge, frame, id, otype);
      }

      inline LV2_Atom_Forge_Ref
      property_head (uint32_t key, uint32_t context)
      {
         return lv2_atom_forge_property_head (&forge, key, context);
      }

      inline void
      pop (AtomForgeFrame *frame)
      {
         lv2_atom_forge_pop(&forge, frame);
      }

      inline LV2_Atom_Forge_Ref
      integer (const int val)
      {
         return lv2_atom_forge_int(&forge,val);
      }

      inline LV2_Atom_Forge_Ref
      raw (const void* data, uint32_t size)
      {
         return lv2_atom_forge_raw (&forge, data, size);
      }

   };
} /* namespace lvtk */

#endif /* LVTK_LV2_ATOM_HPP */
