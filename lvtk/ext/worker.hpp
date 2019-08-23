/* 
    Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/** @defgroup worker Worker 
    Scheduling work.
    <h3>Example</h3>
    @code
        #include <lvtk/plugin.hpp>
        #include <lvtk/ext/worker.hpp>

        using namespace lvtk;
        
        class WorkPlugin : public Plugin<WorkPlugin, Worker> {
        public:
            WorkPlugin (const Args& args) : Plugin (args) {}

            void run (uint32_t nframes) {
                // schedule some work. This just writes a string which isn't that useful
                schedule_work ("workmsg", strlen("workmsg") + 1);
            }

            WorkerStatus work (WorkerRespond &respond, uint32_t size, const void* data) { 
                // perform work
                // send responses with @c respond
                return WORKER_SUCCESS; 
            }

            WorkerStatus work_response (uint32_t size, const void* body) {
                // handle responses send in @c work
                return WORKER_SUCCESS;
            }

            WorkerStatus end_run() {
                // optional: do anything needed at the end of the run cycle    
            }
        };
    @endcode
*/

#pragma once

#include <lvtk/ext/extension.hpp>
#include <lvtk/scheduler.hpp>

namespace lvtk {

/** Adds LV2 worker support to your instance. Add this to your instance's
    Mixin list to activate it.

    @headerfile lvtk/ext/worker.hpp
    @ingroup worker
*/
template<class I> 
struct Worker : Extension<I>
{
    /** @private */
    Worker (const FeatureList& features) { 
        for (const auto& f : features)
            if (schedule.set_feature (f))
                break;
    }

    /** Schedule work with the host
     
        @param size Size of data
        @param data The data to schedule
     */
    WorkerStatus schedule_work (uint32_t size, const void* data) const { return schedule.schedule_work (size, data); }

    /** Perform work as requested by schedule_work
     
        @param respond  Function to send responses with
     */
    WorkerStatus work (WorkerRespond &respond, uint32_t size, const void* data) { return WORKER_SUCCESS; }

    /** Process work responses sent with respond in the `work` callback
     
        @param size Size of response data
        @param data The reponse data
     */
    WorkerStatus work_response (uint32_t size, const void* body) { return WORKER_SUCCESS; }

    /** Called at the end of a processing cycle */
    WorkerStatus end_run() { return WORKER_SUCCESS; }

protected:
    /** @private */
    static void map_extension_data (ExtensionMap& dmap) {
        static const LV2_Worker_Interface _worker = { _work, _work_response, _end_run  };
        dmap[LV2_WORKER__interface] = &_worker;
    }

private:
    Scheduler schedule;
    /** @internal */
    static LV2_Worker_Status _work (LV2_Handle					instance,
                                    LV2_Worker_Respond_Function respond,
                                    LV2_Worker_Respond_Handle   handle,
                                    uint32_t                    size,
                                    const void*                 data)
    {
        WorkerRespond wrsp (instance, respond, handle);
        return (LV2_Worker_Status) (static_cast<I*> (instance))->work (wrsp, size, data);
    }

    /** @internal */
    static LV2_Worker_Status _work_response (LV2_Handle  instance,
                                             uint32_t    size,
                                             const void* body)
    {
        return (LV2_Worker_Status)(static_cast<I*>(instance))->work_response (size, body);
    }

    /** @internal */
    static LV2_Worker_Status _end_run (LV2_Handle instance)
    {
        return (LV2_Worker_Status)(static_cast<I*> (instance))->end_run();
    }
};

}
