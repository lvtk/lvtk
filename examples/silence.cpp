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

using namespace lvtk;
using std::vector;

#define LVTK_SILENCE_URI "http://lvtoolkit.org/plugins/silence"
#define LVTK_SILENCE_PREFIX LVTK_SILENCE_URI "#"
#define LVTK_SILENCE_MSG LVTK_SILENCE_PREFIX "msg"

class Silence : public Instance, 
                public State
{
public:
    Silence (double rate, const std::string& path, const FeatureList& features)
        : Instance (rate, path, features)
    {
        urids.atom_String = map (LV2_ATOM__String);
        urids.midi_type   = map (LV2_MIDI__MidiEvent);
        urids.silence_msg = map (LVTK_SILENCE_MSG);
    }

    void activate()  {}
    void deactivate()  {}

    void connect_port (uint32_t port, void* data)  {
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

    void check_midi ()
    {
        #if 0
        const LV2_Atom_Sequence* midiseq = p<LV2_Atom_Sequence> (p_midi);
        LV2_ATOM_SEQUENCE_FOREACH (midiseq, ev)
        {
            uint32_t frame_offset = ev->time.frames;
            if (ev->body.type == urids.midi_type)
            {
                std::cout << "MIDI\n";
            }
        }
        #endif
    }

    struct SilenceURIs {
        LV2_URID atom_String;
        LV2_URID silence_msg;
        LV2_URID midi_type;
    } urids;
};

static Plugin<Silence> lvtk_Silence (LVTK_SILENCE_URI, { 
    LV2_URID__map } 
);

static StateInterface<Silence>   v2_State;
static Worker<Silence>           lv2_Worker;
