/* 
    Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/** @defgroup atom Atom 
    Dealing with LV2 Atoms
*/

#pragma once

#include <iostream>
#include <string>

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>

namespace lvtk {
/* @{ */
/** Alias to `LV2_Atom_Event`  */
using AtomEvent             = LV2_Atom_Event;
/** Alias to `LV2_Atom_Property_Body` */
using PropertyBody          = LV2_Atom_Property_Body ;
/** Alias to `LV2_Atom_Forge_Frame` */
using ForgeFrame            = LV2_Atom_Forge_Frame;
/** Alias to `LV2_Atom_Forge_Ref` */
using ForgeRef              = LV2_Atom_Forge_Ref;
/** Alias to `LV2_Atom_Object_Query` */
using ObjectQuery           = LV2_Atom_Object_Query;

/** An LV2_Atom_Object wrapper
    @headerfile lvtk/ext/atom.hpp
 */
struct Object
{
    /** Create an Object from raw data. The data passed in will be casted 
        to a LV2_Atom_Object pointer
        
        @param atom     Pointer to an LV2_Atom_Object
     */
    Object (const void* atom) : p_obj ((LV2_Atom_Object*) atom) { }

    /** Create an Object from a ForgeRef 
     
        @param ref  A ForgeRef which will be casted to LV2_Atom_Object internally
    */
    Object (ForgeRef ref) : p_obj ((LV2_Atom_Object*) ref) { }

    /** Copy constructor only references the internal pointer */
    Object (const Object& other)    { operator= (other); }

    /** @returns the object type */
    inline uint32_t otype() const           { return object_type(); }

    /** @returns the object type */
    inline uint32_t object_type() const     { return p_obj->body.otype; }

    /** Return the object's id */
    inline uint32_t id() const              { return p_obj->body.id; }

    /** Return the object's total size */
    inline uint32_t total_size() const      { return lv2_atom_total_size ((LV2_Atom*) p_obj); }

    /** Get an object's values for various keys.

        The value pointer of each item in @p query will be set to the location of
        the corresponding value in @p object.  Every value pointer in @p query MUST
        be initialised to NULL.  This function reads @p object in a single linear
        sweep.  By allocating @p query on the stack, objects can be "queried"
        quickly without allocating any memory.  This method is realtime safe.

        This function can only do "flat" queries, it is not smart enough to match
        variables in nested objects.
    */
    inline void query (ObjectQuery& query) const {
        lv2_atom_object_query (p_obj, &query);
    }

    /** Get the underlying LV2_Atom_Object pointer */
    inline LV2_Atom_Object* c_obj()          const { return p_obj; }
    /** Pass this as a LV2_Atom Object* parameter */
    inline operator LV2_Atom_Object*()       const { return p_obj; }
    /** Pass this as a const LV2_Atom Object* parameter */
    inline operator const LV2_Atom_Object*() const { return p_obj; }

    /** Assignment operator takes reference of the internal pointer */
    inline Object& operator= (const Object& other)
    {
        p_obj = other.p_obj;
        return *this;
    }

    /** @private */
    class iterator
    {
    public:
        iterator (LV2_Atom_Object* o, LV2_Atom_Property_Body* i)
            : index (i), obj (o) { }

        const PropertyBody& operator*()  const { assert (index); return *index; }
        const PropertyBody* operator->() const { assert (index); return index; }

        iterator& operator++()
        {
            index = lv2_atom_object_next (index);
            if (lv2_atom_object_is_end (&obj->body, obj->atom.size, index))
                index = 0;
            return *this;
        }

        iterator operator++ (int)
        {
            iterator ret (obj, index);
            ++(*this);
            return ret;
        }

        bool operator== (const iterator& other) const { return index == other.index; }
        bool operator!= (const iterator& other) const { return index != other.index; }

    private:
        friend struct Object;
        LV2_Atom_Property_Body* index;
        LV2_Atom_Object*        obj;
    };

    /** Start of properties */
    iterator begin() const { return iterator (p_obj, lv2_atom_object_begin (&p_obj->body)); }

    /** End of properties */
    iterator end()   const { return iterator (p_obj, nullptr); }

private:
    LV2_Atom_Object* p_obj = nullptr;
};

/** An LV2_Atom wrapper
    These are intended to be used on the stack
    
    @headerfile lvtk/ext/atom.hpp
 */
struct Atom
{
    /** Create a null Atom */
    Atom () : p_atom (nullptr) {}

    /** Create an Atom from raw data */
    Atom (const void* atom) : p_atom (reinterpret_cast<const LV2_Atom*> (atom))  { }

    /** Create an Atom from a Forge Ref */
    Atom (ForgeRef ref) : p_atom (reinterpret_cast<LV2_Atom*> (ref)) { }

    /** Create an Atom from an AtomEvent */
    Atom (AtomEvent* ev) : p_atom (&ev->body) { }

    /** Create an Atom from a CType reference */
    Atom (const LV2_Atom& ref) : p_atom (&ref) { }

    /** Create an Atom from an Object */
    Atom (const Object& o) : p_atom ((const LV2_Atom*) o.c_obj()) { }

    /** Pad a size to 64 bits */
    inline static uint32_t pad_size (uint32_t size) { return lv2_atom_pad_size (size); }

    /** Determine if the Atom is null */
    inline bool is_null()         const { return lv2_atom_is_null (p_atom); }

    /** Returns true if this atom has type and equals a given value
        @param type The atom POD type
        @param pod_value The value to test
    */
    template<typename POD>
    inline bool has_type_and_equals (uint32_t type, const POD& pod_value) const {
        return this->p_atom->type == type && operator== (pod_value);
    }

    /** Returns true if this atom is an otype when casted to Object */
    inline bool has_object_type (uint32_t otype) const {
        return as_object().otype() == otype;
    }

    /** Get the Atom's body */
    inline void* body()           const { return LV2_ATOM_BODY (p_atom); }

    /** Get the body as a boolean */
    inline bool as_bool()         const { return ((LV2_Atom_Bool*)p_atom)->body > 0; }

    /** Get the body as a float */
    inline float as_float()       const { return ((LV2_Atom_Float*)p_atom)->body; }

    /** Get the body as a double */
    inline double as_double()     const { return ((LV2_Atom_Double*)p_atom)->body; }

    /** Returns the atom casted to LV2_Atom_Object */
    const Object as_object()      const { return Object ((LV2_Atom_Object* ) p_atom); }

    /** Get the body as a string */
    inline const char* as_string() const { return (const char*) LV2_ATOM_BODY (p_atom); }

    /** Get the body as a 32bit integer */
    inline int32_t as_int()       const { return ((LV2_Atom_Int*)p_atom)->body; }

    /** Get the body as a long */
    inline int64_t as_long()      const { return ((LV2_Atom_Long*)p_atom)->body; }

    /** Get the body as a URID */
    inline uint32_t as_urid()     const { return ((LV2_Atom_URID*)p_atom)->body; }

    /** Get this Atom's type */
    inline uint32_t type()        const { return p_atom->type; }

    /** Get the Atom's total size */
    inline uint32_t total_size()  const { return lv2_atom_total_size (p_atom); }

    /** Get the Atom's body size */
    inline uint32_t size()        const { return p_atom->size; }

    /** Get the underlying LV2_Atom pointer */
    inline const LV2_Atom* c_obj() const { return p_atom; }

    /** Castable to bool */
    inline operator bool() const { return ! lv2_atom_is_null (p_atom); }
    
    /** Castable to const LV2_Atom* */
    inline operator const LV2_Atom*() const { return p_atom; }

    /** Reference another atom
        Does NOT make a copy, instead references the other
        Atom's internal pointer
     */
    inline Atom& operator= (const Atom& other)
    {
        p_atom = other.p_atom;
        return *this;
    }

    /** Equality operator */
    inline bool operator== (const Atom& other) const { return lv2_atom_equals (c_obj(), other.c_obj()); }
    /** Inequality operator */
    inline bool operator!= (const Atom& other) const { return ! lv2_atom_equals (c_obj(), other.c_obj()); }

private:
    const LV2_Atom* p_atom = nullptr;
    friend struct Object;

    /** @private Used by has_type_and_equals */
    inline bool operator== (const LV2_URID& u) const { return (((LV2_Atom_URID*)p_atom)->body == u); }
    inline bool operator== (const int32_t& i)  const { return (((LV2_Atom_Int*)p_atom)->body == i); }
    inline bool operator== (const int64_t& l)  const { return (((LV2_Atom_Long*)p_atom)->body == l); }
    inline bool operator== (const float& f)    const { return (((LV2_Atom_Float*)p_atom)->body == f); }
    inline bool operator== (const double& f)   const { return (((LV2_Atom_Double*)p_atom)->body == f); }
};

/** An LV2_Atom_Sequence wrapper.
        
    Since this implements an STL style container, you can use it as follows:
    @code

        Sequence seq (my_lv2_sequence_ptr());
        for (const auto& ev : seq) {
            // handle event
        }

    @endcode
    @headerfile lvtk/ext/atom.hpp
*/
struct Sequence
{
    typedef AtomEvent* pointer;
    typedef AtomEvent& reference;
    typedef const AtomEvent* const_pointer;
    typedef const AtomEvent& const_reference;

    /** Create an Sequence from raw data
        @param seq  Pointer to an LV2_Atom_Sequence 
     */
    Sequence (const void* seq)
        : sequence ((LV2_Atom_Sequence*) seq) { }

    /** Create an AtomSequnce from an LV2_Atom_Sequence
        @param seq The sequence to wrap 
     */
    Sequence (LV2_Atom_Sequence* seq)
        : sequence (seq) { }
    
    /** Create an Sequence from a ForgeRef */
    Sequence (ForgeRef ref)
        : sequence ((LV2_Atom_Sequence*) ref) { }

    /** Reset for writing */
    inline void reset() {
        sequence->atom.size = sizeof (LV2_Atom_Sequence_Body);
    }

    /** Return the sequence body's pad. Currently unused per LV2 spec. */
    inline uint32_t pad() const { return sequence->body.pad; }

    /** Return the sequence's body size 
        @note This method does NOT return the number of events contained in
              the Sequence.  It is the size in terms of bytes.
    */
    inline uint32_t size() const        { return sequence->atom.size; }

    /** Return the sequence's unit */
    inline uint32_t unit() const        { return sequence->body.unit; }

    /** Return the sequence's c-type */
    inline LV2_Atom_Sequence* c_obj()   { return sequence; }

    /** Castable to bool. True if the sequence isn't nullptr */
    inline operator bool() const        { return sequence != 0; }

    /** Castable to LV2_Atom_Sequence */
    inline operator LV2_Atom_Sequence*() const { return sequence; }

    /** @private */
    inline operator uint8_t*() const    { return (uint8_t*) sequence; }

    /** Append an AtomEvent to the end of the sequence.

        Effectively this is the same as lv2_atom_sequence_append, but 
        re-implemented to avoid an extra function call.

        @param ev The event to add 
     */
    inline void append (const AtomEvent& ev) {
        if (AtomEvent* pos = lv2_atom_sequence_end (&sequence->body, sequence->atom.size)) {
            auto total_size = (uint32_t)sizeof(ev) + ev.body.size;
            memcpy (pos, &ev, total_size);
            sequence->atom.size += lv2_atom_pad_size (total_size);
        }
    }

    /** Insert an AtomEvent into the middle of the sequence
        
        @param ev The event to insert 
     */
    inline void insert (const AtomEvent& ev) {
        AtomEvent* e = lv2_atom_sequence_end (&sequence->body, sequence->atom.size);
        LV2_ATOM_SEQUENCE_FOREACH (sequence, iter) {
            if (iter->time.frames > ev.time.frames) {
                memmove (((uint8_t*) iter) + lv2_atom_pad_size ((uint32_t) sizeof(*iter) + iter->body.size),
                         (uint8_t*) iter,
                         (uint8_t*) e - (uint8_t*) iter);
                e = iter;
                break;
            }
        }

        if (e) {
            auto total_size = (uint32_t)sizeof(ev) + ev.body.size;
            memcpy (e, &ev, total_size);
            sequence->atom.size += lv2_atom_pad_size (total_size);
        }
    }

    /** @private */
    class iterator
    {
    public:
        iterator (LV2_Atom_Sequence *seq, AtomEvent* ev) : p_event (ev), sequence (seq) { }
        AtomEvent&  operator*()  { return *p_event; }
        const AtomEvent* operator->() const { return p_event; }

        iterator& operator++()
        {
            p_event = lv2_atom_sequence_next (p_event);
            return *this;
        }

        iterator operator++(int)
        {
            iterator res (sequence, p_event);
            ++(*this);
            return res;
        }

        inline bool operator== (const iterator& other) const { return p_event == other.p_event; }
        inline bool operator!= (const iterator& other) const { return p_event != other.p_event; }

    private:
        friend struct Sequence;
        LV2_Atom_Event* p_event;
        LV2_Atom_Sequence* sequence;
    };

    /** @returns an iterator starting at the first event */
    inline iterator begin() const { 
        return iterator (sequence, lv2_atom_sequence_begin (&sequence->body)); 
    }

    /** @returns the end iterator of this sequence */
    inline iterator end()   const { 
        return iterator (sequence, lv2_atom_sequence_end (&sequence->body, sequence->atom.size)); 
    }

private:
    LV2_Atom_Sequence* sequence = nullptr;
    uint32_t capacity = 0;
};

/** Class wrapper around LV2_Atom_Forge
    @headerfile lvtk/ext/atom.hpp
*/
class Forge : public LV2_Atom_Forge
{
public:
    /** Uninitialized Forge.
        Client code must call Forge::init() before using otherwise
        written output will be unpredictable and likely cause a nasty crash
    */
    Forge() = default;

    /** Initialized Forge.
        @param map The LV2_URID_Map to use for initialization    
     */
    Forge (LV2_URID_Map* map)   { init (map); }

    /** Initialize the underlying atom forge
         @param map The mapping function needed for init
    */
    inline void init (LV2_URID_Map* map) {
        lv2_atom_forge_init (this, map);
    }

    /** Set the Forge's buffer
        
        You must call this before writing Atoms with the forge. Failing
        to do so could result in a nasty crash.

        @param buf The buffer to use
        @param size The size of the buffer
    */
    inline void set_buffer (uint8_t* buf, uint32_t size) {
        lv2_atom_forge_set_buffer (this, buf, size);
    }

    /** Forge the header of a sequence */
    inline ForgeRef write_sequence_head (ForgeFrame& frame, uint32_t unit) {
        return lv2_atom_forge_sequence_head (this, &frame, unit);
    }

    /** Forge frame time (in a sequence) */
    inline ForgeRef write_beat_time (double beats) {
        return lv2_atom_forge_beat_time (this, beats);
    }

    /** Forge frame time (in a sequence). The returned ForgeRef is to an
        LV2_Atom_Event
    */
    inline ForgeRef write_frame_time (int64_t frames) {
        return lv2_atom_forge_frame_time (this, frames);
    }

    /** Write a property header
        @param key The URID for the key
        @param context The context
    */
    inline ForgeRef write_property_head (uint32_t key, uint32_t context) {
        return lv2_atom_forge_property_head (this, key, context);
    }

    /** Pop a forge frame
        @param frame The frame to pop
    */
    inline void pop (ForgeFrame& frame) {
        lv2_atom_forge_pop (this, &frame);
    }

    /** Pop a forge frame
        @param frame The frame to pop
    */
    inline void pop_frame (ForgeFrame& frame) {
        lv2_atom_forge_pop (this, &frame);
    }

    /** Write an atom header

        @param size The atom's body size
        @param type The atom's body type
        @return A reference to the written atom
    */
    inline ForgeRef write_atom (uint32_t size, uint32_t type) {
        return lv2_atom_forge_atom (this, size, type);
    }

    inline ForgeRef write_key (uint32_t urid) {
        return lv2_atom_forge_key (this, urid);
    }

    /** Write an atom object */
    inline ForgeRef write_object (ForgeFrame& frame, uint32_t id, uint32_t otype) {
        return lv2_atom_forge_object (this, &frame, id, otype);
    }

    /** Write an atom path from string

        @param path The path to forge
        @return An Atom
    */
    inline ForgeRef write_path (const std::string& path) {
        return lv2_atom_forge_path (this, path.c_str(), path.size());
    }

    inline ForgeRef write_primitive (const Atom& atom) {
        return lv2_atom_forge_primitive (this, atom.c_obj());
    }

    /** Write an atom resource

        @param frame    Frame position
        @param id       Resource URID
        @param otype    Resource Object Type URID
        @returns        A reference to the Atom
    */
    inline ForgeRef write_resource (ForgeFrame& frame, uint32_t id, uint32_t otype) {
        return lv2_atom_forge_object (this, &frame, id, otype);
    }

    /** Write a blank object
        @param frame    Frame position
        @param id       Object URID
        @param otype    Object Type URID
        @returns        A reference to the Atom
    */
    inline ForgeRef write_blank (ForgeFrame& frame, uint32_t id, uint32_t otype) {
        return lv2_atom_forge_object (this, &frame, id, otype);
    }

    /** Write a boolean value
        @param val  The value to write
        @returns    A reference to the Atom
    */
    inline ForgeRef write_bool (const bool val) {
        return lv2_atom_forge_bool (this, val);
    }

    /** Write an integeger value
        @param val The value to write
        @returns    A reference to the Atom
     */
    inline ForgeRef write_int (const int val) { 
        return lv2_atom_forge_int (this, val); 
    }

    /** Write a double value @param val the value to write
        @param val The value to write
        @returns    A reference to the Atom
     */
    inline ForgeRef write_double (const double val) { 
        return lv2_atom_forge_double (this, val); 
    }

    /** Write a float value */
    inline ForgeRef write_float (const float val) { 
        return lv2_atom_forge_float (this, val); 
    }

    /** Write a long integer value  */
    inline ForgeRef write_long (const int64_t val) { 
        return lv2_atom_forge_long (this, val); 
    }

    /** Write a string value */
    inline ForgeRef write_string (const char* str) { 
        return lv2_atom_forge_string (this, str, strlen (str)); 
    }

    /** Write a URI string */
    inline ForgeRef write_uri (const char* uri) { 
        return lv2_atom_forge_uri (this, uri, strlen (uri)); 
    }

    /** Write raw data
        @param data The data to write
        @param size The size in bytes of data
     */
    inline ForgeRef write_raw (const void* data, uint32_t size) {
        return lv2_atom_forge_raw (this, data, size);
    }

    /** Write a URID value
        @param id The URID to write
     */
    inline ForgeRef write_urid (LV2_URID id) {
        return lv2_atom_forge_urid (this, id);
    }
};

/** An LV2_Atom_Vector Wrapper 
    @headerfile lvtk/ext/atom.hpp
*/
class Vector
{
public:
    inline Vector (ForgeRef ref) : vec ((LV2_Atom_Vector*) ref) { }
    ~Vector() { }

    inline size_t size() const { return vec->atom.size / vec->body.child_size; }
    inline uint32_t child_size() const { return vec->body.child_size; }
    inline uint32_t child_type() const { return vec->body.child_type; }
    inline LV2_Atom_Vector* c_obj() const { return vec; }
    inline operator LV2_Atom_Vector* () const { return vec; }

    /** @private */
    class iterator
    {
    public:
        iterator& operator++()
        {
            offset += vec->body.child_size;

            if (vec && offset >= vec->atom.size)
                offset = vec->atom.size;

            return *this;
        }

        iterator operator++(int)
        {
            iterator it (vec, offset);
            ++(*this);
            return it;
        }

        inline bool operator== (const iterator& other) const { return vec == other.vec && offset == other.offset; }
        inline bool operator!= (const iterator& other) const { return vec != other.vec && offset != other.offset; }

        /** Reference another iterator */
        inline iterator& operator= (const iterator& other)
        {
            this->vec = other.vec;
            this->offset = other.offset;
            return *this;
        }

    private:
        friend class Vector;
        iterator (LV2_Atom_Vector *v, uint32_t os = 0) : vec (v), offset (os) { }
        LV2_Atom_Vector* vec;
        uint32_t offset;
    };

    /** Returns an iterator to the begining of the vector */
    iterator begin() const { return iterator (vec); }

    /** Returns the end iterator */
    iterator end()   const { return iterator (vec, vec->atom.size); }

private:
    LV2_Atom_Vector* vec = nullptr;
};
/* @} */
}  /* namespace lvtk */
