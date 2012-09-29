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

#include <unistd.h>
#include <iostream>
#include <vector>
#include <cstdlib>

#include <daps/synth.hpp>
#include <daps/data_access.hpp>
#include <daps/state.hpp>

using namespace daps;
using std::vector;

class workhorse : public Plugin<workhorse, DataAccess<false>, Worker<true>, State<true> >
{
   public:

    workhorse (double rate)
    : Plugin<workhorse, DataAccess<false>, Worker<true>, State<true> > (3)
    {

    }

    void
    run(uint32_t nframes)
    {
       const char* msg = "go to sleep";
       this->schedule_work(strlen(msg), (void*)msg);
    }

   /* ============================= Worker ============================ */

    worker_status_t
    work_response (uint32_t size, const void* body)
    {
       std::clog << "workhorse: wake up!\n";
       std::clog << "workhorse: msg was '" << (const char*)body << "'\n";
       return WORKER_SUCCESS;
    }

    worker_status_t
    work (worker_respond &respond, uint32_t  size, const void*  data)
    {
       std::clog << "workhorse: taking a nap now\n";
       sleep (3);
       respond (size, data);
       return WORKER_SUCCESS;
    }

};

static int _ = workhorse::register_class ("http://daps-project.org/plugins/workhorse");
