
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>

#include <lvtk/plugin.hpp>
#include <lvtk/ext/atom.hpp>
#include <lv2/log/log.h>

namespace lvtk {

using std::vector;

class Workhorse : public Instance<Workhorse, Worker>
{
public:
    Workhorse (double rate, const String& path, const FeatureList& features)
        : Instance (rate, path, features),
          m_sleeping (false),
          log_Entry (map (LV2_LOG__Entry)),
          log_Trace (map (LV2_LOG__Trace)),
          bufsize (0)
    { }

    void deactivate()  { }
    void connect_port (uint32_t, void* data)  {}
    void activate() 
    {
        #if 0
        // query for buffer information.
        const BufferInfo& info (get_buffer_info());
        std::stringstream ss;
        ss << "Workhorse Buffer Information:\n";
        ss << "\tBuffer Bounded:  " << info.bounded << std::endl
           << "\tBuffer Fixed:    " << info.fixed << std::endl
           << "\tBuffer Pow of 2: " << info.power_of_two << std::endl
           << "\tBuffer Min:      " << info.min << std::endl
           << "\tBuffer Max:      " << info.max << std::endl
           << "\tSequence Size:   " << info.sequence_size << std::endl;
        printf (log_Entry, ss.str().c_str());
        #endif
    }

    void run (uint32_t nframes) 
    {
        static const char* msg = "go to sleep";

        if (! m_sleeping)
        {
            switch (schedule_work (strlen (msg) + 1, (void*) msg))
            {
            case WORKER_SUCCESS:
                // printf (log_Trace, "[workhorse] scheduled a job\n");
                m_sleeping = true;
                break;
            default:
                // printf (log_Trace, "[workhorse] unknown scheduling error\n");
                break;
            }
        }
    }

    /* ============================= Worker ============================ */

    /** This is executed by the host after work executes a respond
        object. */
    WorkerStatus work_response (uint32_t size, const void* body)
    {
        /** Print message with LV2 Log */
        // printf (log_Trace, "[workhorse] woke up. message: %s\n", (char*)body);
        std::clog << "scheduled_work \n";
        m_sleeping = false;
        return WORKER_SUCCESS;
    }

    /** Do some work ...
        This gets called from the host after schedule_work
        is called in run */
    WorkerStatus work (WorkerRespond &respond, uint32_t  size, const void*  data)
    {
        /** Print message with LV2 Log's printf */
        // printf (log_Entry, "[workhorse] taking a nap now\n");
        sleep (1);

        std::clog << "working.... " << std::endl;

        /** Send a response */
        respond (size, data);
        return WORKER_SUCCESS;
    }

    WorkerStatus end_run()  { return WORKER_SUCCESS; }

#if 0
    uint32_t get_options (Option*)
    {
        // Don't have a host for this yet
        return OPTIONS_SUCCESS;
    }

    uint32_t set_options (const Option*)
    {
        // Don't have a host for this yet
        return OPTIONS_SUCCESS;
    }
#endif

private:
    bool m_sleeping;

    /** This is used for LV2 Log demonstration */
    LV2_URID log_Entry;
    LV2_URID log_Trace;

    uint32_t bufsize;
};

#if 1


static Plugin<Workhorse> workhorse ("http://lvtoolkit.org/plugins/workhorse", {
    LV2_URID__map, LV2_WORKER__schedule
});

#endif

}
