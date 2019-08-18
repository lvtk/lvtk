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
   @file volume.cpp
 */


#include <math.h>
#include <lvtk/plugin.hpp>

#define LVTK_VOLUME_URI "http://lvtoolkit.org/plugins/volume"

namespace lvtk {

class Volume : public Instance<Volume>
{
public:
   #if LVTK_STATIC_ARGS
    Volume (const Args& args) { }
   #else
    Volume (const Args& args) : Instance (args) { }
   #endif

    void activate() { }
    void deactivate() { }

    void connect_port (uint32_t port, void* data)  {
        if (port == 0)
            input[0] = (float*) data;
        else if (port == 1)
            input[1] = (float*) data;
        else if (port == 2)
            output[0] = (float*) data;
        else if (port == 3)
            output[1] = (float*) data;
        else if (port == 4 && data != nullptr)
            db = *(float*) data;
    }

    void run (uint32_t nframes) { 
        const float gain = db > -90.0f ? powf (10.0f, db * 0.05f) : 0.0f;
        for (uint32_t f = 0; f < nframes; ++f)
            for (uint32_t c = 0; c < 2; ++c)
                output[c][f] = input[c][f] * gain;
    }

private:
    float* input[2] { 0, 0 };
    float* output[2] { 0, 0 };
    float db = 0.0;
};

static Plugin<Volume> volume (LVTK_VOLUME_URI, {
    LV2_URID__map ///* < required host feature
});

}
