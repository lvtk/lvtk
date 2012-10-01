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

#include <lvtk/plugin.hpp>
#include <lvtk/log.hpp>
#include <lvtk/urid.hpp>

/** This is a ttl2c generated header */
#include "workhorse.h"

using namespace lvtk;
using std::vector;

#define MIXINS URID<true>, Log<true>, Worker<true>

class workhorse : public Plugin<workhorse, MIXINS >
{
   public:

    workhorse (double rate)
    : Plugin<workhorse, MIXINS > (3)
    , m_sleeping (false)
    {
    	msgType = map(LV2_LOG__Entry);
    }

    void
    run(uint32_t nframes)
    {
        /** Arbitrary message. Normally you'd want to send
    	    an LV2 Atom of sorts for scheduling */
       const char* msg = "go to sleep";

       if (!m_sleeping)
       {
		   /** Schedule a job with msg as the data */
		   WorkerStatus status = schedule_work (strlen(msg), (void*)msg);

		   switch (status)
		   {
			   case WORKER_SUCCESS:
				   printf (msgType,"[worker] scheduled a job\n");
				   break;
			   default:
				   printf (msgType,"[worker] unknown scheduling error\n");
				   break;

		   }
       }
    }

   /* ============================= Worker ============================ */


    /**
       This is executed by the host after work executes a respond
       object.
     */
    WorkerStatus
    work_response (uint32_t size, const void* body)
    {
    	/** Print message with LV2 Log */
        printf (msgType, "[workhorse] woke up. message: %s\n", (char*)body);

        return WORKER_SUCCESS;
    }


    /**
       Do some work
       This gets called from the host after schedule_work
       is called in run
     */
    WorkerStatus
    work (WorkerRespond &respond, uint32_t  size, const void*  data)
    {
    	/** Print message with LV2 Log's printf */
        printf (msgType, "[workhorse] taking a nap now\n");
        m_sleeping = true;
        sleep (3);
        m_sleeping = false;

        /** Send a response */
        respond (size, data);

        return WORKER_SUCCESS;
    }

   private:

    bool m_sleeping;

    /** This is used for LV2 Log demonstration */
    LV2_URID msgType;

};

static int _ = workhorse::register_class (p_uri);

