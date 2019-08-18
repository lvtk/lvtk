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

#include <lvtk/state_functions.hpp>
#include <lvtk/ext/extension.hpp>

namespace lvtk {

/** Adds LV2 State support to your plugin instance
    @ingroup interfaces
*/
template<class I>
struct State : Extension<I>
{   
    /** @private */
    State (const FeatureList&) { }

    /** Called by the host to save state
     
        @param store    Store function object to write keys/values
        @param flags    State flags to check
        @param features Additional features for this operation 
    */
    StateStatus save (StateStore &store, uint32_t flags, const FeatureList &features) {  return STATE_SUCCESS; }

    /** Called by the host to restore state
     
        @param retrieve Retrieve function object to get keys/values
        @param flags    State flags to check
        @param features Additional features for this operation 
    */
    StateStatus restore (StateRetrieve &retrieve, uint32_t flags, const FeatureList &features) {  return STATE_SUCCESS; }

protected:
    /** @internal */
    inline static void map_extension_data (ExtensionMap& dmap) {
        static const LV2_State_Interface _state =  { _save, _restore };
        dmap[LV2_STATE__interface] = &_state;
    }

private:
    static LV2_State_Status _save (LV2_Handle                    instance,
                                   LV2_State_Store_Function      store_function,
                                   LV2_State_Handle              state_handle,
                                   uint32_t                      flags,
                                   const LV2_Feature *const *    features)
    {
        auto* const plugin = reinterpret_cast<I*> (instance);
        StateStore store (store_function, state_handle);
        FeatureList flist (features);
        return (LV2_State_Status) plugin->save (store, flags, flist);
    }

    static LV2_State_Status _restore (LV2_Handle                  instance,
                                      LV2_State_Retrieve_Function retrieve_function,
                                      LV2_State_Handle            handle,
                                      uint32_t                    flags,
                                      const LV2_Feature *const *  features)
    {
        auto* const plugin = static_cast<I*> (instance);
        StateRetrieve retrieve (retrieve_function, handle);
        FeatureList feature_set (features);
        return (LV2_State_Status) plugin->restore (retrieve, flags, feature_set);
    }
};

}
