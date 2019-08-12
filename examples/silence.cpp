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

#include <lvtk/plugin.hpp>

#define LVTK_SILENCE_URI "http://lvtoolkit.org/plugins/silence"
#define LVTK_SILENCE_PREFIX LVTK_SILENCE_URI "#"
#define LVTK_SILENCE_MSG LVTK_SILENCE_PREFIX "msg"

namespace lvtk {

class Silence : public Instance<Silence, State, Log, BufSize>
{
public:
    Silence (double rate, const std::string& path, const FeatureList& features)
        : Instance (rate, path, features)
    {
        urids.atom_String = map (LV2_ATOM__String);
        urids.silence_msg = map (LVTK_SILENCE_MSG);
        auto info = buffer_details();
    }

    void activate()  {}
    void deactivate()  {}

    void connect_port (uint32_t port, void* data) {
        if (port == 0)
            audio = (float*) data;
    }

    void run (uint32_t nframes)  {
        memset (audio, 0, sizeof(float) * nframes);
    }

    StateStatus save (StateStore &store, uint32_t flags, const FeatureList &features)
    {
        const char* msg = "Sorry I can't hear you. Please speak up";
        return store (urids.silence_msg, (void*) msg,
                      strlen (msg) + 1, urids.atom_String,
                      STATE_IS_POD | STATE_IS_PORTABLE);
    }

    StateStatus restore (StateRetrieve &retrieve, uint32_t flags, const FeatureList &features) 
    {
        size_t size;
        uint32_t type, fs;

        const void *message = retrieve (urids.silence_msg, &size, &type, &fs);
        if (message)
        {
            std::cout << "[lvtk] " << (char*) message << std::endl;
            return STATE_SUCCESS;
        }

        return STATE_ERR_UNKNOWN;
    }

private:
    float* audio = nullptr;

    struct SilenceURIs {
        LV2_URID atom_String;
        LV2_URID silence_msg;
    } urids;
};

using SilencePlugin = Plugin<Silence>;
static SilencePlugin silence (
    LVTK_SILENCE_URI, {
        LV2_URID__map
    }
);

}
