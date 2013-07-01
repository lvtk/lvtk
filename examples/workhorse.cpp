/**
  workhorse.cpp  -  LV2 Toolkit - Plugin Example

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
 * Demonstration of LV2 Worker, LV2 URID, LV2 Options, and LV2 Log in C++
 */

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>

#include <lvtk/plugin.hpp>

/* This is a ttl2c generated header */
#include "workhorse.h"

using namespace lvtk;
using std::vector;




class Workhorse;
typedef lvtk::Plugin<Workhorse, URID<true>, Options<false>, BufSize<false>,
                                Log<false>, Worker<true> >  PluginType;

class Workhorse : public PluginType
{
public:

    Workhorse (double rate)
        : PluginType (p_n_ports),
          m_sleeping (false),
          entryType (map (LV2_LOG__Entry)),
          traceType (map (LV2_LOG__Trace)),
          bufsize (0)
    { }

    void
    activate()
    {
        // query for buffer information.
        const BufferInfo& info (get_buffer_info());
        std::stringstream ss;
        ss << "Buffer Bounded:  " << info.bounded << std::endl
           << "Buffer Fixed:    " << info.fixed << std::endl
           << "Buffer Pow of 2: " << info.power_of_two << std::endl
           << "Buffer Min:      " << info.min << std::endl
           << "Buffer Max:      " << info.max << std::endl
           << "Sequence Size:   " << info.sequence_size << std::endl;
        printf (entryType, ss.str().c_str());
    }

    void
    run(uint32_t nframes)
    {
        static const char* msg = "go to sleep";

        if (! m_sleeping)
        {
            /** Schedule a job with msg as the data */
            WorkerStatus status (schedule_work (strlen(msg) + 1, (void*)msg));

            switch (status)
            {
            case WORKER_SUCCESS:
                printf (traceType, "[workhorse] scheduled a job\n");
                m_sleeping = true;
                break;
            default:
                printf (traceType, "[workhorse] unknown scheduling error\n");
                break;
            }
        }
    }

    /* ============================= Worker ============================ */

    /** This is executed by the host after work executes a respond
        object. */
    WorkerStatus
    work_response (uint32_t size, const void* body)
    {
        /** Print message with LV2 Log */
        printf (traceType, "[workhorse] woke up. message: %s\n", (char*)body);
        m_sleeping = false;
        return WORKER_SUCCESS;
    }

    /** Do some work ...
        This gets called from the host after schedule_work
        is called in run */
    WorkerStatus
    work (WorkerRespond &respond, uint32_t  size, const void*  data)
    {
        /** Print message with LV2 Log's printf */
        printf (entryType, "[workhorse] taking a nap now\n");
        sleep (10);

        /** Send a response */
        respond (size, data);
        return WORKER_SUCCESS;
    }

    uint32_t
    get_options (Option*)
    {
        // Don't have a host for this yet
        return OPTIONS_SUCCESS;
    }

    uint32_t
    set_options (const Option*)
    {
        // Don't have a host for this yet
        return OPTIONS_SUCCESS;
    }

private:

    bool m_sleeping;

    /** This is used for LV2 Log demonstration */
    LV2_URID entryType;
    LV2_URID traceType;

    uint32_t bufsize;

};

static int _ = Workhorse::register_class (p_uri);

