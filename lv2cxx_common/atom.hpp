/*
  atom.hpp  -  Definition of the Class

    2012             Michael Fisher <mfisher31@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
/** 
 * @file atom.hpp
 * This file contains definition of the Class
 */

#ifndef LV2_ATOM_HPP
#define LV2_ATOM_HPP


#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>


namespace LV2 {

   /**
    * The Atom Feature.
    * The actual type that your plugin class will inherit when you use
    * this mixin is the internal struct template I.
    * @ingroup pluginmixins
    */
    template <bool Required = true>
    struct Atom {

      /** This is the type that your plugin class will inherit when you use the
          EventRef mixin. The public and protected members defined here
          will be available in your plugin class.
      */
      template <class Derived> struct I : Extension<Required> {

        /** @internal */
        I() : m_callback_data(0), m_ref_func(0), m_unref_func(0) { }

        /** @internal */
        static void map_feature_handlers(FeatureHandlerMap& hmap) {
          hmap[LV2_ATOM_URI] = &I<Derived>::handle_feature;
        }

        /** @internal */
        static void handle_feature(void* instance, void* data) {
          Derived* d = reinterpret_cast<Derived*>(instance);
          I<Derived>* fe = static_cast<I<Derived>*>(d);
          fe->m_ok = true;
        }

        bool check_ok() {
          if (LV2CXX_DEBUG) {
            std::clog<<"    [LV2::EventRef] Validation "
                     <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
          }
          return this->m_ok;
        }
      };
    };
}

#endif /* LV2_ATOM_HPP */
