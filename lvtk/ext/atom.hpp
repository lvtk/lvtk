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
   typedef LV2_Atom_Forge_Frame ForgeFrame;
   typedef LV2_Atom_Forge_Ref   ForgeRef;

   /** Function type for mapping symbols */
   typedef uint32_t     (*MapFunc)(const char* symbol);

   /** Function type for unmaping URIDs */
   typedef const char*  (*UnmapFunc)(uint32_t id);


   struct Atom;


   struct AtomObject
   {
       typedef LV2_Atom_Property_Body Property;

       typedef std::vector<LV2_Atom_Property_Body*> PropertyVec;


      AtomObject (const void* atom) : p_obj ((LV2_Atom_Object*) atom) { }
      AtomObject (const AtomObject& other) : p_obj (other.p_obj)   { }

      LV2_Atom_Object* cobj() const     { return p_obj; }
      operator LV2_Atom_Object*()       { return p_obj; }

      Atom atom(); //                       { return (LV2_Atom*) p_obj; }

      LV2_URID otype() const            { return p_obj->body.otype; }

      uint32_t id() const               { return p_obj->body.id; }

      uint32_t total_size() const
      {
          return lv2_atom_total_size ((LV2_Atom*) p_obj);
      }

      inline void
      query (LV2_Atom_Object_Query* query)
      {
          lv2_atom_object_query (p_obj, query);
      }

      void print ()
      {
          LV2_ATOM_OBJECT_FOREACH (p_obj, iter)
        {
             // std::cout<< "key: " << iter->key << std::endl;

        }
      }

      inline AtomObject&
      operator= (const AtomObject& other)
      {
         p_obj = other.p_obj;
         return *this;
      }

   private:

      LV2_Atom_Object* p_obj;

   };


   struct Atom
   {
      Atom () : p_atom (NULL) { }
      Atom (const void* atom) : p_atom ((LV2_Atom*) atom)  { }
      Atom (ForgeRef ref) : p_atom ((LV2_Atom*) ref) { }
      Atom (LV2_Atom_Event* ev) : p_atom (&ev->body) { }

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

      inline int64_t
      as_long() const
      {
          return ((LV2_Atom_Long*)p_atom)->body;
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


   struct AtomEvent
   {
       AtomEvent (const void* ev) : event ((LV2_Atom_Event*) ev) { }

       uint32_t type() const { return event->body.type; }
       uint32_t size() const { return event->body.size; }
       uint32_t time_frames() const { return event->time.frames; }
       uint32_t frames() const { return time_frames(); }
       double time_beats() const { return event->time.beats; }
       const LV2_Atom* body() const { return &event->body; }

       Atom atom() { return Atom (&event->body); }

   private:
       LV2_Atom_Event* event;
   };


   struct AtomSequence
   {
       AtomSequence (const void* slab) : seq ((LV2_Atom_Sequence*) slab) { }

       uint32_t size() const { return seq->atom.size; }

       inline LV2_Atom_Sequence* cobj() { return seq; }

       inline operator uint8_t*() const { return (uint8_t*) seq; }

   private:
       LV2_Atom_Sequence* seq;
   };


   /** Class wrapper around LV2_Atom_Forge */
   class AtomForge
   {
      LV2_Atom_Forge forge;

   public:

      typedef ForgeRef   Ref;

      typedef LV2_Atom_Forge_Frame Frame;

      /** Uninitialized AtomForge.

          @note Client code must call AtomForge::init() before using otherwise
          written output will be unpredictable
       */
      AtomForge() { }

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
      }


      /** Get the underlying atom forge
          @return The forge
       */
      inline LV2_Atom_Forge*
      cobj()
      {
         return &forge;
      }

      /** */
      inline ForgeRef
      sequence_head (ForgeFrame& frame, uint32_t unit)
      {
          return lv2_atom_forge_sequence_head (&forge, &frame, unit);
      }

      inline operator LV2_Atom_Forge* () { return cobj(); }

      /** Set the forge's buffer

          @param buf The buffer to use
          @param size The size of the buffer
       */
      inline void
      set_buffer (uint8_t* buf, uint32_t size)
      {
         lv2_atom_forge_set_buffer (&forge, buf, size);
      }

      /** Forge frame time (in a sequence) */
      inline ForgeRef
      frame_time (int64_t frames)
      {
          return lv2_atom_forge_frame_time (&forge, frames);
      }

      /** Forge frame time (in a sequence) */
      inline ForgeRef
      beat_time (double beats)
      {
          return lv2_atom_forge_beat_time (&forge, beats);
      }

      /** Forge an atom header

          @param size The atom's body size
          @param type The atom's body type
          @return A reference to the written atom
       */
      inline ForgeRef
      atom (uint32_t size, uint32_t type)
      {
          return lv2_atom_forge_atom (&forge, size, type);
      }

      /** Forge an atom path from string

          @param path The path to forge
          @return An Atom
       */
      inline ForgeRef
      path (const std::string& path)
      {
         return lv2_atom_forge_path (&forge, path.c_str(), path.size());
      }

      /** Forge an atom resource

          @param frame
          @param id
          @param otype
          @return A reference to the Atom
       */
      inline ForgeRef
      resource (AtomForgeFrame& frame, uint32_t id, uint32_t otype)
      {
         // Write object header
         return lv2_atom_forge_resource (&forge, &frame, id, otype);
      }

      inline ForgeRef
      blank (AtomForgeFrame& frame, uint32_t id, uint32_t otype)
      {
         // Write object header
         return lv2_atom_forge_blank (&forge, &frame, id, otype);
      }

      inline ForgeRef
      property_head (uint32_t key, uint32_t context)
      {
         return lv2_atom_forge_property_head (&forge, key, context);
      }

      inline void
      pop (AtomForgeFrame& frame)
      {
         lv2_atom_forge_pop (&forge, &frame);
      }

      inline ForgeRef
      write_int (const int val)
      {
          return lv2_atom_forge_int (&forge, val);
      }


      inline ForgeRef
      write_float (const float val)
      {
          return lv2_atom_forge_float (&forge, val);
      }

      inline ForgeRef
      write_long (const int64_t val)
      {
          return lv2_atom_forge_long (&forge, val);
      }


      inline ForgeRef
      raw (const void* data, uint32_t size)
      {
         return lv2_atom_forge_raw (&forge, data, size);
      }

      inline ForgeRef
      urid (LV2_URID id)
      {
          return lv2_atom_forge_urid (&forge, id);
      }

   };

   inline Atom
   AtomObject::atom()
   {
       return Atom ((LV2_Atom*) p_obj);
   }

} /* namespace lvtk */

#endif /* LVTK_ATOM_HPP */
