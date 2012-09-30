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

#ifndef DAPS_LV2_RESIZE_PORT_HPP
#define DAPS_LV2_RESIZE_PORT_HPP

#include <lv2/lv2plug.in/ns/ext/resize-port/resize-port.h>

#include <daps/types.hpp>
#include <daps/util.hpp>

namespace daps {

	/** Resize port status codes */
	typedef enum {
		RESIZE_PORT_SUCCESS      = LV2_RESIZE_PORT_SUCCESS,		/**< Completed successfully. */
		RESIZE_PORT_ERR_UNKNOWN  = LV2_RESIZE_PORT_ERR_UNKNOWN, /**< Unknown Error */
		RESIZE_PORT_ERR_NO_SPACE = LV2_RESIZE_PORT_ERR_NO_SPACE /**< Insufficient space */
	} resize_port_status_t;

   /**
      The Data Access Extension.
      The actual type that your plugin class will inherit when you use
      this mixin is the internal struct template I.
      @ingroup pluginmixins
   */
   DAPS_MIXIN_CLASS ResizePort {
      DAPS_MIXIN_DERIVED {

         I() : p_resize_port_resize (NULL) { }


         /* ================= Mixin API ========================= */


         /** @internal */
         static void
         map_feature_handlers(feature_handler_map& hmap)
         {
           hmap[LV2_RESIZE_PORT__resize] = &I<Derived>::handle_feature;
         }

         /** @internal */
         static void
         handle_feature (daps::handle instance, feature_data data)
         {
            Derived* derived = reinterpret_cast<Derived*>  (instance);
            I<Derived>* mixin = static_cast<I<Derived>*> (derived);

            mixin->p_resize_port_resize =
            		reinterpret_cast<LV2_Resize_Port_Resize*> (data);

            mixin->m_ok = true;
         }

         bool
         check_ok()
         {
            if (DAPS_DEBUG) {
              std::clog<<"    [LV2::ResizePort] Validation "
                       <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
            }
            return this->m_ok;
         }

      protected:

        /* ================= Resize Port C++ Implementation =============== */

        /**
		   Resize a port buffer to at least @a size bytes.

		   This function MAY return an error, in which case the port buffer
		   was not resized and the port is still connected to the same location.  Plugins
		   MUST gracefully handle this situation.

		   This function is in the audio threading class.

		   The host MUST preserve the contents of the port buffer when resizing.

		   Plugins MAY resize a port many times in a single run callback.  Hosts
		   SHOULD make this as inexpensive as possible.
		*/
		resize_port_status_t
		resize (uint32_t index, size_t size)
		{
			if (NULL == p_resize_port_resize)
			{
				return RESIZE_PORT_ERR_UNKNOWN;
			}

			LV2_Resize_Port_Feature_Data data =
								p_resize_port_resize->data;

			return (resize_port_status_t)
					p_resize_port_resize->resize(data, index, size);
		}

        LV2_Resize_Port_Resize * p_resize_port_resize;
      };
   };

} /* namespace daps */

#endif /* DAPS_LV2_RESIZE_PORT_HPP */
