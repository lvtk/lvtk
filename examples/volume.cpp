/*
    silence.cpp  -  LV2 Toolkit API Demonstration Plugin

    Copyright (C) 2012  Michael Fisher <mfisher31@gmail.com>

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
   @file silence.cpp
   Demonstrates LV2 URID mapping, Log, and State Save/Restore
 */


#include <iostream>
#include <vector>
#include <cstdlib>

#include <lvtk/plugin.hpp>

#include <lvtk/ext/atom.hpp>
#include <lv2/midi/midi.h>
#include <lvtk/feature.hpp>
#include <lvtk/ext/state.hpp>
#include <lvtk/ext/urid.hpp>
#include <lvtk/ext/worker.hpp>

#define LVTK_VOLUME_URI "http://lvtoolkit.org/plugins/volume"

namespace lvtk {

using std::vector;

class Volume : public Instance<Volume>
{
public:
    Volume (double rate, const std::string& path, const FeatureList& features)
        : Instance (rate, path, features) { }

    void activate()  { }
    void deactivate()  { }
    void connect_port (uint32_t port, void* data)  {}
    void run (uint32_t nframes)  { }
};

using VolumePlugin = Plugin<Volume>;
static VolumePlugin silence (LVTK_VOLUME_URI);

}
