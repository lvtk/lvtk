/*
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
*/
/** @file atom.hpp */
/** @page atom LVTK Atom
    C++ convenience wrappers for LV2 Atoms<br />
 */

#ifndef LVTK_ATOM_HPP
#define LVTK_ATOM_HPP

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>

namespace lvtk {

   /** Typedefs for an Atom data types */
   typedef LV2_Atom_Event        AtomEvent;
   typedef LV2_Atom_Forge_Frame  ForgeFrame;
   typedef LV2_Atom_Forge_Ref    ForgeRef;
   typedef LV2_Atom_Object_Query ObjectQuery;


   /** Basic wrapper for an LV2_Atom_Object
       This class is intended to be created on the stack
    */
   struct AtomObject
   {
      /** Create an AtomObject from raw data */
      AtomObject (const void* atom) : p_obj ((LV2_Atom_Object*) atom) { }

      /** Create an AtomObject from a ForgeRef */
      AtomObject (ForgeRef ref) : p_obj ((LV2_Atom_Object*) ref) { }

      AtomObject (const AtomObject& other) : p_obj (other.p_obj) { }

      /** Return the object type */
      inline uint32_t
      otype() const
      {
          return p_obj->body.otype;
      }

      /** Return the object's id */
      inline uint32_t
      id() const
      {
          return p_obj->body.id;
      }

      /** Return the object's total size */
      inline uint32_t
      total_size() const
      {
          return lv2_atom_total_size ((LV2_Atom*) p_obj);
      }

      /**
         Get an object's values for various keys.

         The value pointer of each item in @p query will be set to the location of
         the corresponding value in @p object.  Every value pointer in @p query MUST
         be initialised to NULL.  This function reads @p object in a single linear
         sweep.  By allocating @p query on the stack, objects can be "queried"
         quickly without allocating any memory.  This method is realtime safe.

         This function can only do "flat" queries, it is not smart enough to match
         variables in nested objects.
      */
      inline void
      query (ObjectQuery& query)
      {
          lv2_atom_object_query (p_obj, &query);
      }

      /** @internal */
      inline LV2_Atom_Object* cobj() const     { return p_obj; }

      /** @internal */
      inline operator LV2_Atom_Object*()       { return p_obj; }

      /** @internal */
      inline AtomObject&
      operator= (const AtomObject& other)
      {
         p_obj = other.p_obj;
         return *this;
      }

   private:

      LV2_Atom_Object* p_obj;

   };


   /** Basic wrapper for an LV2_Atom
       These are intended to be used on the stack
    */
   struct Atom
   {
      /** Create a null Atom */
      Atom () : p_atom (0) { }

      /** Create an Atom from raw data */
      Atom (const void* atom) : p_atom ((LV2_Atom*) atom)  { }

      /** Create an Atom from a Forge Ref */
      Atom (ForgeRef ref) : p_atom ((LV2_Atom*) ref) { }

      /** Create an Atom from an AtomEvent */
      Atom (AtomEvent* ev) : p_atom (&ev->body) { }

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

      /** Get the body as a float */
      inline float
      as_float() const
      {
          return ((LV2_Atom_Float*)p_atom)->body;
      }

      /** Returns the atom casted to LV2_Atom_Object */
      const AtomObject
      as_object() const {
          return AtomObject ((LV2_Atom_Object* ) p_atom);
      }

      /** Get the body as a string */
      inline const char*
      as_string() const
      {
          return (const char*) LV2_ATOM_BODY (p_atom);
      }

      /** Get the body as a float */
      inline int
      as_int() const
      {
          return ((LV2_Atom_Int*)p_atom)->body;
      }

      /** Get the body as a long */
      inline int64_t
      as_long() const
      {
          return ((LV2_Atom_Long*)p_atom)->body;
      }

      /** Get the body as a URID */
      inline uint32_t
      as_urid() const
      {
          return ((LV2_Atom_URID*)p_atom)->body;
      }
      /** Get this Atom's type */

      inline uint32_t
      type() const
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

      /** @internal */
      inline operator const LV2_Atom*() { return cobj(); }

      /** @internal */
      inline Atom&
      operator= (const Atom& other)
      {
         p_atom = other.p_atom;
         return *this;
      }

      /** @internal */
      inline bool
      operator== (Atom& other)
      {
          return lv2_atom_equals (cobj(), other.cobj());
      }

   private:

      const LV2_Atom* p_atom;
      friend class AtomObject;

   };


   /** Basic wrapper for an LV2_Atom_Sequence */
   struct AtomSequence
   {
       /** Create an AtomSequence from raw data */
       AtomSequence (const void* slab) : p_seq ((LV2_Atom_Sequence*) slab) { }

       /** Create an AtomSequnce from an LV2_Atom_Sequence */
       AtomSequence  (LV2_Atom_Sequence* seq) : p_seq (seq) { }

       /** Create an AtomSequence from a ForgeRef */
       AtomSequence (ForgeRef ref) : p_seq ((LV2_Atom_Sequence*) ref) { }

       /** Return the sequence body's pad */
       inline uint32_t
       pad() const
       {
           return p_seq->body.pad;
       }

       /** Return the sequence's body size */
       inline uint32_t
       size() const
       {
           return p_seq->atom.size;
       }

       /** Return the sequences unit */
       inline uint32_t
       unit() const
       {
           return p_seq->body.unit;
       }

       /** Return the sequences c-type */
       inline LV2_Atom_Sequence*
       cobj()
       {
           return p_seq;
       }

       /** @skip */
       inline operator LV2_Atom_Sequence*() const { return p_seq; }

       /** @skip */
       inline operator uint8_t*() const { return (uint8_t*) p_seq; }

   private:

       LV2_Atom_Sequence* p_seq;
   };


   /** Class wrapper around LV2_Atom_Forge */
   class AtomForge
   {
   public:

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

      /** Forge the header of a sequence */
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
      beat_time (double beats)
      {
          return lv2_atom_forge_beat_time (&forge, beats);
      }

      /** Forge frame time (in a sequence). The return ForgeRef is to an
          LV2_Atom_Event
       */
      inline ForgeRef
      frame_time (int64_t frames)
      {
          return lv2_atom_forge_frame_time (&forge, frames);
      }

      inline ForgeRef
      property_head (uint32_t key, uint32_t context)
      {
         return lv2_atom_forge_property_head (&forge, key, context);
      }

      inline void
      pop (ForgeFrame& frame)
      {
         lv2_atom_forge_pop (&forge, &frame);
      }

      /** Write an atom header

          @param size The atom's body size
          @param type The atom's body type
          @return A reference to the written atom
       */
      inline ForgeRef
      write_atom (uint32_t size, uint32_t type)
      {
          return lv2_atom_forge_atom (&forge, size, type);
      }

      /** Write an atom path from string

          @param path The path to forge
          @return An Atom
       */
      inline ForgeRef
      write_path (const std::string& path)
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
      write_resource (ForgeFrame& frame, uint32_t id, uint32_t otype)
      {
          // Write object header
          return lv2_atom_forge_resource (&forge, &frame, id, otype);
      }

      inline ForgeRef
      write_blank (ForgeFrame& frame, uint32_t id, uint32_t otype)
      {
          // Write object header
          return lv2_atom_forge_blank (&forge, &frame, id, otype);
      }

      inline ForgeRef
      write_bool (const bool val)
      {
          return lv2_atom_forge_bool (&forge, val);
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
      write_string (const char* str)
      {
          return lv2_atom_forge_string (&forge, str, strlen (str));
      }

      inline ForgeRef
      write_uri (const char* uri)
      {
          return lv2_atom_forge_uri (&forge, uri, strlen (uri));
      }

      inline ForgeRef
      write_raw (const void* data, uint32_t size)
      {
          return lv2_atom_forge_raw (&forge, data, size);
      }

      inline ForgeRef
      write_urid (LV2_URID id)
      {
          return lv2_atom_forge_urid (&forge, id);
      }

   private:

      LV2_Atom_Forge forge;
   };

} /* namespace lvtk */

#endif /* LVTK_ATOM_HPP */
