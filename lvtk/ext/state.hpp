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

#pragma once

#include <cassert>
#include <lv2/lv2plug.in/ns/ext/state/state.h>

namespace lvtk {

typedef enum {
    /** Plain Old Data.

        Values with this flag contain no pointers or references to other areas
        of memory.  It is safe to copy POD values with a simple memcpy and store
        them for the duration of the process.  A POD value is not necessarily
        safe to trasmit between processes or machines (e.g. filenames are POD),
        see STATE_IS_PORTABLE for details.

        Implementations MUST NOT attempt to copy or serialise a non-POD value if
        they do not understand its type (and thus know how to correctly do so).
        */

    STATE_IS_POD = LV2_STATE_IS_POD,

    /** Portable (architecture independent) data.

        Values with this flag are in a format that is usable on any
        architecture.  A portable value saved on one machine can be restored on
        another machine regardless of architecture.  The format of portable
        values MUST NOT depend on architecture-specific properties like
        endianness or alignment.  Portable values MUST NOT contain filenames.
        */
    STATE_IS_PORTABLE = LV2_STATE_IS_PORTABLE,

    /** Native data.

        This flag is used by the host to indicate that the saved data is only
        going to be used locally in the currently running process (e.g. for
        instance duplication or snapshots), so the plugin should use the most
        efficient representation possible and not worry about serialisation
        and portability.
        */
    STATE_IS_NATIVE = LV2_STATE_IS_NATIVE
} StateFlags;

typedef enum {
    STATE_SUCCESS         = LV2_STATE_SUCCESS,          /**< Completed successfully. */
    STATE_ERR_UNKNOWN     = LV2_STATE_ERR_UNKNOWN,      /**< Unknown error. */
    STATE_ERR_BAD_TYPE    = LV2_STATE_ERR_BAD_TYPE,     /**< Failed due to unsupported type. */
    STATE_ERR_BAD_FLAGS   = LV2_STATE_ERR_BAD_FLAGS,    /**< Failed due to unsupported flags. */
    STATE_ERR_NO_FEATURE  = LV2_STATE_ERR_NO_FEATURE,   /**< Failed due to missing features. */
    STATE_ERR_NO_PROPERTY = LV2_STATE_ERR_NO_PROPERTY   /**< Failed due to missing property. */
} StateStatus;


/** Wrapper struct for state retrieval. This wraps an
    LV2_State_Retrieve_Function and exeucutes via operator () 
    @headerfile lvtk/ext/state.hpp
 */
struct StateRetrieve
{
    StateRetrieve (LV2_State_Retrieve_Function srfunc, LV2_State_Handle handle)
        : p_handle (handle), p_srfunc (srfunc) { }

    /** Execute the retrieve functor.
        @param key
        @param size
        @param type
        @param flags
        @return Associate 'value' data for the given key
      */
    const void* operator () (uint32_t key, 
                             size_t *size = nullptr,
                             uint32_t *type  = nullptr,
                             uint32_t *flags = nullptr) const
    {
        return p_srfunc (p_handle, key, size, type, flags);
    }

private:
    LV2_State_Handle              p_handle;
    LV2_State_Retrieve_Function   p_srfunc;
};

/**  Wrapper struct for state storage. This wraps an
     LV2_State_Store_Function and exeucutes via operator () 
 */
struct StateStore
{
    StateStore (LV2_State_Store_Function ssfunc, LV2_State_Handle handle)
        : p_handle(handle), p_ssfunc(ssfunc) { }

    /** Execute the store functor.

        @param key
        @param value
        @param size
        @param type
        @param flags
        @return STATE_SUCCESS on Success
        */
    inline StateStatus operator() (uint32_t key, const void* value,
                                    size_t size, uint32_t type,
                                    uint32_t flags = 0) const
    {
        return (StateStatus) p_ssfunc (p_handle, key, value, size, type, flags);
    }

private:
    LV2_State_Handle              p_handle;
    LV2_State_Store_Function      p_ssfunc;
};

} /* namespace lvtk */
