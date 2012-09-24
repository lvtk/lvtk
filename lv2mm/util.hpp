/**
    util.hpp - Support file for writing LV2 plugins in C++

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
   @util.hpp
   Utilities and helpers for implementing LV2 in C++.
*/

#ifndef LV2_UTIL_HPP
#define LV2_UTIL_HPP

namespace util {

#if 0
   /** WIP : Not used yet and does not compile
      Cast a LV2::Handle in to a feature mixin
      @param instance The LV2::Handle object
      @return The feature's mixin struct
      @ingroup utilities
    */
   template <class Derived, class I>
   I<Derived>*
   mixin_cast (LV2::Handle instance)
   {
      Derived* derived = reinterpret_cast<Derived*>  (instance);
      I<Derived>* mixin = static_cast<I<Derived>*> (derived);
      return mixin;
   }
#endif

} /* namespace util */

#endif /* LV2_UTIL_HPP */
