/*
  workhorse.cpp  -  DAPS LV2++ Worker Demonstration

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
 * @file workhorse.cpp
 */

#include <iostream>
#include <vector>
#include <cstdlib>

#include <daps/plugin.hpp>
#include <daps/state.hpp>

using namespace daps;
using std::vector;

#define DAPS_SILENCE_URI "http://daps-project.org/plugins/silence"
#define DAPS_SILENCE_PREFIX DAPS_SILENCE_URI "#"
#define DAPS_SILENCE_MSG DAPS_SILENCE_PREFIX "msg"

class silence : public Plugin<silence, URID<true>, State<true> >
{
   public:

    silence (double rate)
    : Plugin<silence, URID<true>, State<true> > (1)
    {
       urids.atom_String = map(LV2_ATOM__String);
       urids.silence_msg = map(DAPS_SILENCE_MSG);
    }

    void
    run(uint32_t nframes)
    {
       float *out = p(0);
       for (int i = 0; i < nframes; i++)
       {
          out[i] = 0.0f;
       }
    }


    state_status_t
    save (state_store &store, uint32_t flags,
                     feature_vec &features)
    {
       const char* msg = "Sorry I can't hear you. Please speak up.";
       return store (urids.silence_msg, msg,
                     strlen(msg), urids.atom_String,
                     STATE_IS_POD | STATE_IS_PORTABLE);
    }


    state_status_t
    restore(state_retrieve &retrieve, uint32_t flags,
                        const feature_vec &features)
    {
       size_t size;
       uint32_t type,fs;

       const void *st = retrieve (urids.silence_msg, &size, &type, &fs);
       if (!st)
       {
         std::cout << (char*)st << std::endl;
       }

       return STATE_SUCCESS;
    }

   private:
    struct silence_urids {
       LV2_URID atom_String;
       LV2_URID silence_msg;
    } urids;

};

static int _ = silence::register_class (DAPS_SILENCE_URI);
