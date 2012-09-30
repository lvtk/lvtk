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
 * Demonstration of LV2 Worker and LV2 Log in C++
 */

#include <unistd.h>
#include <iostream>
#include <vector>
#include <cstdlib>

#include <daps/synth.hpp>
#include <daps/data_access.hpp>
#include <daps/log.hpp>
#include <daps/state.hpp>

#include "workhorse.h"

using namespace daps;
using std::vector;

#define MIXINS URID<true>, Log<true>, Worker<true>

class workhorse : public Plugin<workhorse, MIXINS >
{
   public:

    workhorse (double rate)
    : Plugin<workhorse, MIXINS > (3)
    {
    	msgType = map("http://daps-project.org/plugins/workhorse#logger");
    }

    void
    run(uint32_t nframes)
    {
    	/** Arbitrary message. Normally you'd want to send
    	    an LV2 Atom of sorts for scheduling */
       const char* msg = "go to sleep";

       /** Schedule a job with msg as the data */
       this->schedule_work(strlen(msg), (void*)msg);
    }

   /* ============================= Worker ============================ */


    /**
       This is executed by the host after work executes a respond
       object.
     */
    worker_status_t
    work_response (uint32_t size, const void* body)
    {
    	/** Print message with LV2 Log */
        printf (msgType, "[workhorse] woke up. message was : %s\n", (char*)body);

        return WORKER_SUCCESS;
    }


    /**
       Do some work
       This gets called from the host after schedule_work
       is called in run
     */
    worker_status_t
    work (worker_respond &respond, uint32_t  size, const void*  data)
    {
    	/** Print message with LV2 Log's printf */
        printf (msgType, "[workhorse] taking a nap now\n");
        sleep (3);

        /** Send a response */
        respond (size, data);

        return WORKER_SUCCESS;
    }

   private:

    /** This is used for LV2 Log demonstration */
    LV2_URID msgType;

};

static int _ = workhorse::register_class ("http://daps-project.org/plugins/workhorse");
