/**
    patch.hpp - Support file for writing LV2 plugins in C++

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
   @patch.hpp
   C++ convenience header for the LV2 patch extension.
   LV2 Version: 1.0 (2012-04-17)

   This vocabulary defines messages which can be used to access and
   manipulate properties. It is designed to provide a dynamic control
   interface for LV2 plugins, but is useful in many contexts.

   This specification is strictly metadata and does not define any binary
   mechanism, though it can be completely expressed by standard types in
   the LV2 Atom extension. Thus, hosts can be expected to be capable of
   transmitting it between plugins, or between a plugin and its UI, making
   it a good choice for advanced plugin control.

   @url http://lv2plug.in/ns/ext/patch
*/

#ifndef LV2_PATCH_HPP
#define LV2_PATCH_HPP

#include <lv2/lv2plug.in/ns/ext/patch/patch.h>

namespace LV2 {

} /* namespace LV2 */

#endif /* LV2_PATCH_HPP */
