
#include <unistd.h>
#include <sstream>
#include <lvtk/plugin.hpp>

#define LVTK_WORKHORSE_URI  "http://lvtoolkit.org/plugins/workhorse"

namespace lvtk {

using std::vector;

class Workhorse : public Instance<Workhorse, Worker, Log, BufSize>
{
public:
    Workhorse (double rate, const std::string& path, const FeatureList& features)
        : Instance (rate, path, features),
          m_sleeping (false),
          log_Entry (map (LV2_LOG__Entry)),
          log_Trace (map (LV2_LOG__Trace)),
          bufsize (0)
    { }

    void deactivate()  { }
    void connect_port (uint32_t, void* data)  { }
    void activate()
    {
        // query for buffer information.
        const auto& info (buffer_details());
        std::stringstream ss;
        ss << "Workhorse Buffer Information:\n";
        ss << "\tBounded:  " << info.bounded << std::endl
           << "\tFixed:    " << info.fixed << std::endl
           << "\tPower of 2: " << info.power_of_two << std::endl
           << "\tMin:      " << info.min << std::endl
           << "\tMax:      " << info.max << std::endl
           << "\tSequence Size:   " << info.sequence_size << std::endl;
        log << ss.str();
    }

    void run (uint32_t nframes) 
    {
        static const char* msg = "go to sleep";

        if (! m_sleeping)
        {
            switch (schedule_work (strlen (msg) + 1, (void*) msg))
            {
            case WORKER_SUCCESS:
                log.printf (log_Trace, "scheduled a job\n");
                m_sleeping = true;
                break;
            default:
                log.printf (log_Trace, "unknown scheduling error\n");
                break;
            }
        }
    }

    /** This is executed by the host after work executes a respond
        object. */
    WorkerStatus work_response (uint32_t size, const void* body)
    {
        /** Print message with LV2 Log */
        log.printf (log_Trace, "woke up. message: %s\n", (char*)body);
        m_sleeping = false;
        return WORKER_SUCCESS;
    }

    /** Do some work ...
        This gets called from the host after schedule_work
        is called in run */
    WorkerStatus work (WorkerRespond &respond, uint32_t  size, const void*  data)
    {
        /** Print message with LV2 Log's printf */
        log.printf (log_Entry, "taking a nap now\n");
        sleep (1);

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


static Plugin<Workhorse> workhorse (LVTK_WORKHORSE_URI, {
    LV2_URID__map, LV2_WORKER__schedule
});

#endif

}
