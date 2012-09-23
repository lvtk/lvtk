/****************************************************************************

    state.hpp - support file for writing LV2 plugins in C++

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

#ifndef LV2_STATE_HPP
#define LV2_STATE_HPP

#include <lv2/lv2plug.in/ns/ext/state/state.h>

namespace LV2 {

   typedef enum {
      /**
         Plain Old Data.

         Values with this flag contain no pointers or references to other areas
         of memory.  It is safe to copy POD values with a simple memcpy and store
         them for the duration of the process.  A POD value is not necessarily
         safe to trasmit between processes or machines (e.g. filenames are POD),
         see LV2_STATE_IS_PORTABLE for details.

         Implementations MUST NOT attempt to copy or serialise a non-POD value if
         they do not understand its type (and thus know how to correctly do so).
      */
      STATE_IS_POD = LV2_STATE_IS_POD,

      /**
         Portable (architecture independent) data.

         Values with this flag are in a format that is usable on any
         architecture.  A portable value saved on one machine can be restored on
         another machine regardless of architecture.  The format of portable
         values MUST NOT depend on architecture-specific properties like
         endianness or alignment.  Portable values MUST NOT contain filenames.
      */
      STATE_IS_PORTABLE = LV2_STATE_IS_PORTABLE,

      /**
         Native data.

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

  /**
   * The State Feature.
   * The actual type that your plugin class will inherit when you use
   * this mixin is the internal struct template I.
   * @ingroup pluginmixins
   */
   template <bool Required = true>
   struct State {

     /** This is the type that your plugin class will inherit when you use the
         EventRef mixin. The public and protected members defined here
         will be available in your plugin class.
     */
     template <class Derived> struct I : Extension<Required> {

       /** @internal */
       I() { }

       /** @internal */
       static void map_feature_handlers(FeatureHandlerMap& hmap) {

       }

       /** @internal */
       static void handle_feature(void* instance, void* data) {

       }

       bool check_ok() {
         if (LV2CXX_DEBUG) {
           std::clog<<"    [LV2::State] Validation "
                    <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
         }
         return this->m_ok;
       }

       /** @internal */
       static const void* extension_data (const char* uri) {
         if (!std::strcmp (uri, LV2_STATE__interface)) {
           static LV2_State_Interface state = { &I<Derived>::_save,
                                                &I<Derived>::_restore };
           return &state;
         }
         return 0;
       }

       StateStatus save(LV2_State_Store_Function   store,
                        LV2_State_Handle           handle,
                        uint32_t                   flags,
                        const Feature *const * features)
       {
          return STATE_SUCCESS;
       }

       StateStatus restore(LV2_State_Retrieve_Function retrieve,
                           LV2_State_Handle            handle,
                           uint32_t                    flags,
                           const Feature *const *  features)
       {
          return STATE_SUCCESS;
       }

     protected:

       /** LV2 Api Implementation */

       /**
        * @internal
        * called from host
        */
       static LV2_State_Status _save(LV2_Handle                 instance,
                                     LV2_State_Store_Function   store,
                                     LV2_State_Handle           handle,
                                     uint32_t                   flags,
                                     const LV2_Feature *const * features)
       {
         Derived* plugin = reinterpret_cast<Derived*>(instance);
         return (LV2_State_Status)plugin->save(store, handle,flags,features);
       }

       /**
        * @internal
        * called from host
        */
       static LV2_State_Status _restore(LV2_Handle                  instance,
                                        LV2_State_Retrieve_Function retrieve,
                                        LV2_State_Handle            handle,
                                        uint32_t                    flags,
                                        const LV2_Feature *const *  features)
       {
         Derived* plugin = reinterpret_cast<Derived*>(instance);
         return (LV2_State_Status)plugin->restore(retrieve, handle,flags,features);
       }

     };
   };

} /* namespace LV2 */

#endif /* LV2_STATE_HPP */
