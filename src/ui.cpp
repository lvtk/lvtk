/****************************************************************************

 ui.cpp - Wrapper library to make it easier to write LV2 UIs in C++

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA  02110-1301  USA

 ****************************************************************************/

#include <cstring>
#include <iostream>

#include <lvtk/ui.hpp>

using namespace std;

namespace lvtk
{

    UIDescList::~UIDescList()
    {
      for (unsigned i = 0; i < size(); ++i)
        free((void*)operator[](i).URI);
    }

    UIDescList&
    get_lv2g2g_descriptors()
    {
        static UIDescList list;
        return list;
    }

} /* namespace lvtk */

using namespace lvtk;

extern "C"
{

    const LV2UI_Descriptor*
    lv2ui_descriptor(uint32_t index)
    {
        UIDescList& descs = get_lv2g2g_descriptors();
        if (index < descs.size()) return &descs[index];

        return NULL;
    }

} /* extern "C" */

