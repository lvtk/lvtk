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

#include "silence.h"

using namespace lvtk;
using std::vector;

#define LVTK_SILENCE_URI "http://lvtoolkit.org/plugins/silence"
#define LVTK_SILENCE_PREFIX LVTK_SILENCE_URI "#"
#define LVTK_SILENCE_MSG LVTK_SILENCE_PREFIX "msg"

class Silence : public Plugin<Silence, URID<true>, State<true> >
{
public:

    Silence (double rate)
    : Plugin<Silence, URID<true>, State<true> > (1)
      {
        urids.atom_String = map (LV2_ATOM__String);
        urids.silence_msg = map (LVTK_SILENCE_MSG);
        urids.midi_type   = map (LV2_MIDI__MidiEvent);
      }

    void
    run(uint32_t nframes)
    {
        float *out = p(0);

        check_midi();

        for (int i = 0; i < nframes; i++)
        {
            out[i] = 0.0f;
        }
    }


    StateStatus
    save (StateStore &store, uint32_t flags, const FeatureVec &features)
    {
        const char* msg = "Sorry I can't hear you. Please speak up";

        return store (urids.silence_msg, (void*)msg,
                strlen(msg), urids.atom_String,
                STATE_IS_POD | STATE_IS_PORTABLE);
    }


    StateStatus
    restore (StateRetrieve &retrieve, uint32_t flags, const FeatureVec &features)
    {
        size_t size;
        uint32_t type,fs;

        const void *message = retrieve (urids.silence_msg, &size, &type, &fs);
        if (message)
        {
            std::cout << "[silence] " << (char*)message << std::endl;
            return STATE_SUCCESS;
        }

        return STATE_ERR_UNKNOWN;
    }

private:

    void
    check_midi ()
    {
        const LV2_Atom_Sequence* midiseq = p<LV2_Atom_Sequence> (p_midi);
        LV2_ATOM_SEQUENCE_FOREACH(midiseq, ev)
        {
            uint32_t frame_offset = ev->time.frames;
            if (ev->body.type == urids.midi_type)
            {
                std::cout << "MIDI\n";
            }
        }
    }

    struct SilenceURIs {
        LV2_URID atom_String;
        LV2_URID silence_msg;
        LV2_URID midi_type;
    } urids;

};

static int _ = Silence::register_class (LVTK_SILENCE_URI);
