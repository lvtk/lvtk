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

/** @defgroup state State
    Save and restore state
*/

#pragma once

#include <lv2/state/state.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {

/** Alias of LV2_State_Flags
    @ingroup state
    @headerfile lvtk/ext/state.hpp
*/
using StateFlags = LV2_State_Flags;

/** Alias of LV2_State_Status
    @ingroup state
    @headerfile lvtk/ext/state.hpp
 */
using StateStatus = LV2_State_Status;

/** Wrapper struct for state retrieval. This wraps an
    LV2_State_Retrieve_Function and exeucutes via operator () 
    @ingroup state
    @headerfile lvtk/ext/state.hpp
 */
struct StateRetrieve {
    StateRetrieve (LV2_State_Retrieve_Function srfunc, LV2_State_Handle handle)
        : p_handle (handle), p_srfunc (srfunc) {}

    /** Call the retrieve function

        @param key
        @param size
        @param type
        @param flags
        @returns Associated 'value' data for the given key
      */
    const void* operator() (uint32_t key,
                            size_t* size = nullptr,
                            uint32_t* type = nullptr,
                            uint32_t* flags = nullptr) const {
        return p_srfunc (p_handle, key, size, type, flags);
    }

private:
    LV2_State_Handle p_handle;
    LV2_State_Retrieve_Function p_srfunc;
};

/** Wrapper struct for state storage. This wraps an
    LV2_State_Store_Function and exeucutes via operator () 
    @ingroup state
    @headerfile lvtk/ext/state.hpp
 */
struct StateStore {
    StateStore (LV2_State_Store_Function ssfunc, LV2_State_Handle handle)
        : p_handle (handle), p_ssfunc (ssfunc) {}

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
                                   uint32_t flags = 0) const {
        return (StateStatus) p_ssfunc (p_handle, key, value, size, type, flags);
    }

private:
    LV2_State_Handle p_handle;
    LV2_State_Store_Function p_ssfunc;
};

/** Adds LV2 State support to your plugin instance
    @ingroup state
    @headerfile lvtk/ext/state.hpp
*/
template <class I>
struct State : Extension<I> {
    /** @private */
    State (const FeatureList&) {}

    /** Called by the host when saving state
     
        @param store    Store function object to write keys/values
        @param flags    State flags to check
        @param features Additional features for this operation 
    */
    StateStatus save (StateStore& store,
                      uint32_t flags, const FeatureList& features) {
        return LV2_STATE_SUCCESS;
    }

    /** Called by the host when restoring state
     
        @param retrieve Retrieve function object to get keys/values
        @param flags    State flags to check
        @param features Additional features for this operation 
    */
    StateStatus restore (StateRetrieve& retrieve,
                         uint32_t flags,
                         const FeatureList& features) {
        return LV2_STATE_SUCCESS;
    }

protected:
    /** @private */
    inline static void map_extension_data (ExtensionMap& dmap) {
        static const LV2_State_Interface _state = { _save, _restore };
        dmap[LV2_STATE__interface] = &_state;
    }

private:
    static LV2_State_Status _save (LV2_Handle instance,
                                   LV2_State_Store_Function store_function,
                                   LV2_State_Handle state_handle,
                                   uint32_t flags,
                                   const LV2_Feature* const* features) {
        auto* const plugin = reinterpret_cast<I*> (instance);
        StateStore store (store_function, state_handle);
        FeatureList flist (features);
        return (LV2_State_Status) plugin->save (store, flags, flist);
    }

    static LV2_State_Status _restore (LV2_Handle instance,
                                      LV2_State_Retrieve_Function retrieve_function,
                                      LV2_State_Handle handle,
                                      uint32_t flags,
                                      const LV2_Feature* const* features) {
        auto* const plugin = static_cast<I*> (instance);
        StateRetrieve retrieve (retrieve_function, handle);
        FeatureList feature_list (features);
        return (LV2_State_Status) plugin->restore (retrieve, flags, feature_list);
    }
};

} // namespace lvtk
