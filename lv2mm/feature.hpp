/**
    feature.hpp - Support file for writing LV2 plugins in C++

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
   @feature.hpp
   C++ convenience header for LV2 Feature handling.
   LV2 C Version Support: 1.0.0
*/

#ifndef LV2_FEATURE_HPP
#define LV2_FEATURE_HPP

#include <map>
#include <string>
#include <vector>

namespace LV2 {

   using std::map;
   using std::vector;

   /** Typedef for the LV2_Feature type so we get it into the LV2 namespace. */
   typedef LV2_Feature Feature;

   /** Convenient typedef for a vector of Features. */
   typedef vector<const Feature*> FeatureSet;

   /** Convenient typedef for the feature handler function type. */
   typedef void(*FeatureHandler)(void*, void*);

   /** Convenient typedef for the feature handler map type. */
   typedef map<string, FeatureHandler> FeatureHandlerMap;

} /* namespace LV2 */

#endif /* LV2_FEATURE_HPP */
