/**
    data_access.hpp - support file for writing LV2 plugins in C++

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
/**
   @file data_access.hpp
   C++ convenience header for the LV2 data access extension.
   LV2 C Version Support: 1.6 (2012-04-17)

   This mixin implements the LV2_Extension_Data_Feature.  It provides
   access to LV2_Descriptor::extension_data() for plugin UIs or other
   potentially remote users of a plugin via a data_access() method.
*/

#ifndef LVTK_LV2_DATA_ACCESS_HPP
#define LVTK_LV2_DATA_ACCESS_HPP

#include <lv2/lv2plug.in/ns/ext/data-access/data-access.h>

#include <lvtk/types.hpp>
#include <lvtk/util.hpp>

namespace lvtk {

   /**
      The Data Access Mixin
      @class lvtk::DataAccess
      @ingroup pluginmixins
   */
   LVTK_MIXIN_CLASS DataAccess {
      LVTK_MIXIN_DERIVED {

         I() : p_da (NULL) { }


         /* ================= Mixin API ========================= */


         /** @internal */
         static void
         map_feature_handlers(feature_handler_map& hmap)
         {
           hmap[LV2_DATA_ACCESS_URI] = &I<Derived>::handle_feature;
         }

         /** @internal */
         static void
         handle_feature (lvtk::handle instance, feature_data data)
         {
            Derived* derived = reinterpret_cast<Derived*>  (instance);
            I<Derived>* mixin = static_cast<I<Derived>*> (derived);

            mixin->p_da = reinterpret_cast<LV2_Extension_Data_Feature*> (data);
            mixin->m_ok = (mixin->p_da != NULL);
         }

         bool
         check_ok()
         {
            if (LVTK_DEBUG) {
              std::clog<<"    [LV2::DataAccess] Validation "
                       <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
            }
            return this->m_ok;
         }

         /* ================= Data Access C++ Implementation =============== */


         /**
             A UI can call this to get data (of a type specified by some other
             extension) from the plugin.

             This call never is never guaranteed to return anything, UIs should
             degrade gracefully if direct access to the plugin data is not possible
             (in which case this function will return NULL).

             This is for access to large data that can only possibly work if the UI
             and plugin are running in the same process.  For all other things, use
             the normal LV2 UI communication system.

             @param uri The uri string to query
             @return Not NULL on Success
          */
         const void*
         data_access(const char *uri)
         {
            if (NULL != p_da) {
               return p_da->data_access(uri);
            }
            return NULL;
         }

     protected:
         /** @internal Feature Data passed from host */
         LV2_Extension_Data_Feature   *p_da;
     };
   };

} /* namespace lvtk */

#endif /* LVTK_LV2_DATA_ACCESS_HPP */
