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

#include <lvtk/ext/state.hpp>
#include <lvtk/interface/interface.hpp>

namespace lvtk {

/**
 * The State Interface
 * @headerfile lvtk/ext/state.hpp
 * @see The internal struct I for API details
 * @ingroup pluginmixins
 */
template<class I>
struct State : Interface<I>
{   
    State (const FeatureList&) { }

    StateStatus save (StateStore &store, uint32_t flags, const FeatureList &features) {  return STATE_SUCCESS; }
    StateStatus restore (StateRetrieve &retrieve, uint32_t flags, const FeatureList &features) {  return STATE_SUCCESS; }

protected:
    inline static void map_extension_data (ExtensionMap& dmap) {
        static const LV2_State_Interface _state =  { _save, _restore };
        dmap[LV2_STATE__interface] = &_state;
    }

private:
    /** @internal */
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

    /** @internal - called from host */
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
