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

#ifndef LVTK_LV2_STATE_HPP
#define LVTK_LV2_STATE_HPP

/** TODO: Add map and make path */

#include <lv2/lv2plug.in/ns/ext/state/state.h>

namespace lvtk {

   typedef enum {
      /**
         Plain Old Data.

         Values with this flag contain no pointers or references to other areas
         of memory.  It is safe to copy POD values with a simple memcpy and store
         them for the duration of the process.  A POD value is not necessarily
         safe to trasmit between processes or machines (e.g. filenames are POD),
         see STATE_IS_PORTABLE for details.

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
       Wrapper struct for state retrieval. This wraps an
       LV2_State_Retrieve_Function and exeucutes via operator ()
    */
   struct StateRetrieve {
      StateRetrieve(LV2_State_Retrieve_Function srfunc, LV2_State_Handle handle)
      : p_handle(handle), p_srfunc(srfunc) { }

      /**
          Execute the retrieve functor.
          @param key
          @param size
          @param type
          @param flags
          @return Associate 'value' data for the given key
       */
      const void* operator () (uint32_t key, size_t *size  = NULL,
                                           uint32_t *type  = NULL,
                                           uint32_t *flags = NULL) const
      {
         return p_srfunc(p_handle, key, size, type, flags);
      }

   private:
      LV2_State_Handle              p_handle;
      LV2_State_Retrieve_Function   p_srfunc;
   };

   /* A little redundant */

   /**
      Wrapper struct for state storage. This wraps an
      LV2_State_Store_Function and exeucutes via operator ()
   */
   struct StateStore {
      StateStore (LV2_State_Store_Function ssfunc, LV2_State_Handle handle)
      : p_handle(handle), p_ssfunc(ssfunc) { }

      /**
          Execute the store functor.
          @param key
          @param value
          @param size
          @param type
          @param flags
          @return STATE_SUCCESS on Success
       */
      StateStatus operator () (uint32_t key, const void* value,
                                             size_t   size,
                                             uint32_t type,
                                             uint32_t flags = 0) const
      {
         return (StateStatus) p_ssfunc(
                      p_handle, key, value, size, type, flags);
      }

   private:
      LV2_State_Handle              p_handle;
      LV2_State_Store_Function      p_ssfunc;
   };

  /**
   * The State Mixin
   * @headerfile lvtk/ext/state.hpp
   * @see The internal struct I for API details
   * @ingroup pluginmixins
   */
   template <bool Required = true>
   struct State
   {
	   template <class Derived>
	   struct I : Extension<Required>
	   {

         I() : p_make_path(NULL) {}

         /** @internal */
         static void
         map_feature_handlers (feature_handler_map& hmap)
         {
            /** Setup makePath here. mapPath is intended for
                use in LV2_State_Interface methods only **/
            hmap[LV2_STATE__makePath] = &I<Derived>::handle_make_feature;
         }

         /** @internal */
         static void
         handle_make_feature(void* instance, FeatureData data)
         {
            Derived* d = reinterpret_cast<Derived*>(instance);
            I<Derived>* mixin = static_cast<I<Derived>*>(d);
            mixin->p_make_path =
                  reinterpret_cast<LV2_State_Make_Path*> (data);
         }

         bool
         check_ok()
         {
            this->m_ok = (p_make_path != NULL);

            if (LVTK_DEBUG) {
               std::clog<<"    [LV2::State] Validation "
                                       <<(this->m_ok ? "succeeded" : "failed")<<"."
                                       <<std::endl;
            }
            return this->m_ok;
         }

         /** @internal */
         static const void*
         extension_data (const char* uri) {
            if (!std::strcmp (uri, LV2_STATE__interface)) {
              static LV2_State_Interface state = { &I<Derived>::_save,
                                                   &I<Derived>::_restore };
              return &state;
            }
            return 0;
         }


       /* ===============  State C++ Interface ==========================  */


       StateStatus
       save (StateStore &store, uint32_t flags,
    		                                   const FeatureVec &features)
       {
          return STATE_SUCCESS;
       }

       StateStatus
       restore (StateRetrieve &retrieve, uint32_t flags,
    		   	   	   	   	   	   	   	   	   const FeatureVec &features)
       {
          return STATE_SUCCESS;
       }

     protected:

#if 0
       SAVEME - Map Path is for the State interface methods only.

        char*
        abstract_path (const char* absolute_path);

        char*
        absolute_path(const char* abstract_path)
        {
           return p_map_path->absolute_path (p_map_path->handle, abstract_path);
        }
#endif

       /**
           Return a path the plugin may use to create a new file.
           @param path The path of the new file within a namespace unique to this
           plugin instance.
           @return The absolute path to use for the new file.

           This function can be used by plugins to create files and directories,
           either at state saving time (if this feature is passed to
           LV2_State_Interface.save()) or any time (if this feature is passed to
           LV2_Descriptor.instantiate()).

           The host MUST do whatever is necessary for the plugin to be able to
           create a file at the returned path (e.g. using fopen), including
           creating any leading directories.

           If this function is passed to LV2_Descriptor.instantiate(), it may be
           called from any non-realtime context.  If it is passed to
           LV2_State_Interface.save(), it may only be called within the dynamic
           scope of that function call.

           The caller is responsible for freeing the returned value with free().
        */
       char*
       path (const char* path)
       {
          return p_make_path->path (p_make_path->handle, path);
       }


      /* ==============  LV2 Boiler Plate Implementation ================= */

       LV2_State_Make_Path * p_make_path;

      /** @internal - called from host */
      static LV2_State_Status _save(LV2_Handle                 instance,
                                    LV2_State_Store_Function   store,
                                    LV2_State_Handle           handle,
                                    uint32_t                   flags,
                                    const LV2_Feature *const * features)
      {
         Derived* plugin = reinterpret_cast<Derived*>(instance);

         StateStore ss (store, handle);

         FeatureVec feature_set;
         for (int i = 0; features[i]; ++i) {
            feature_set.push_back (features[i]);
         }

         return (LV2_State_Status)plugin->save(ss, flags, feature_set);
      }

      /** @internal - called from host */
      static LV2_State_Status _restore(LV2_Handle                  instance,
                                       LV2_State_Retrieve_Function retrieve,
                                       LV2_State_Handle            handle,
                                       uint32_t                    flags,
                                       const LV2_Feature *const *  features)
      {
         Derived* plugin = reinterpret_cast<Derived*>(instance);

         /** Initialize a state retrieval callable */
         StateRetrieve sr (retrieve, handle);

         /** Populate a feature vector */
         FeatureVec feature_set;
         for (int i = 0; features[i]; ++i) {
          feature_set.push_back (features[i]);
         }

         return (LV2_State_Status)plugin->restore(sr, flags, feature_set);
      }
     };
   };

} /* namespace lvtk */

#endif /* LVTK_LV2_STATE_HPP */
