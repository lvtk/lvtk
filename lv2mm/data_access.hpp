/****************************************************************************

    urid.hpp - support file for writing LV2 plugins in C++

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

#ifndef LV2_DATA_ACCESS_HPP
#define LV2_DATA_ACCESS_HPP

#include <lv2/lv2plug.in/ns/ext/data-access/data-access.h>

#include <lv2mm/types.hpp>

namespace LV2 {

   /**
      The URID Extension.
      The actual type that your plugin class will inherit when you use
      this mixin is the internal struct template I.
      @ingroup pluginmixins
   */

   LV2MM_MIXIN_CLASS DataAccess {

      LV2MM_MIXIN_DERIVED {

         typedef I<Derived> Mixin;

         I() : p_da (NULL) { }


         /* ================= Mixin API ========================= */


         /** @internal */
         static void
         map_feature_handlers(FeatureHandlerMap& hmap)
         {
           hmap[LV2_DATA_ACCESS_URI] = &Mixin::handle_feature;
         }

         /** @internal */
         static void
         handle_feature (void* instance, void* data)
         {
            Derived* d = reinterpret_cast<Derived*> (instance);
            Mixin* feature = static_cast<Mixin*> (d);
            feature->p_da = reinterpret_cast<LV2_Extension_Data_Feature*> (data);
            feature->m_ok = true;
         }

         bool
         check_ok()
         {
            if (LV2MM_DEBUG) {
              std::clog<<"    [UI::DataAccess] Validation "
                       <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
            }
            return this->m_ok;
         }

         /* ================= Data Access C++ Implementation =============== */


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
}

#endif /* LV2_DATA_ACCESS_HPP */
