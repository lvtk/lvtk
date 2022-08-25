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
    Scheduling work and processing jobs
    <h3>Example</h3>
    @code
        #include <lvtk/ext/worker.hpp>
        #include <lvtk/plugin.hpp>

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

#include <lv2/worker/worker.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {

/** Alias of LV2_Worker_Status
    @ingroup worker
    @headerfile lvtk/ext/worker.hpp
*/
using WorkerStatus = LV2_Worker_Status;

/** Worker reponse function

    This wraps an LV2_Worker_Respond_Function.  It is passed to
    the work method on your Instance

    @ingroup worker
    @headerfile lvtk/ext/worker.hpp
 */
struct WorkerRespond {
    WorkerRespond (LV2_Handle instance,
                   LV2_Worker_Respond_Function respond_function,
                   LV2_Worker_Respond_Handle handle)
        : p_instance (instance),
          p_handle (handle),
          f_respond (respond_function) {}

    /** Execute the worker respond function.
        @param size
        @param data
        @return WORKER_SUCCESS on success
     */
    WorkerStatus operator() (uint32_t size, const void* data) const {
        return f_respond (p_handle, size, data);
    }

private:
    LV2_Handle p_instance;
    LV2_Worker_Respond_Handle p_handle;
    LV2_Worker_Respond_Function f_respond;
};

/** Schedule jobs with the host.

    This wraps LV2_Worker_Schedule.  Used by the Worker interface to add
    `schedule_work` to a plugin instance.  You shouldn't need to use it
    directly. @see @ref Worker

    @ingroup worker
    @headerfile lvtk/ext/worker.hpp
 */
struct WorkerSchedule final : FeatureData<LV2_Worker_Schedule> {
    WorkerSchedule() : FeatureData (LV2_WORKER__schedule) {}

    /** Schedule work with the host
        
        @param size Size of the data
        @param data The data to write
     */
    WorkerStatus operator() (uint32_t size, const void* data) const {
        if (this->data != nullptr)
            return this->data->schedule_work (this->data->handle, size, data);
        return LV2_WORKER_ERR_UNKNOWN;
    }
};

/** Adds LV2 worker support to your instance. Add this to your instance's
    Mixin list to activate it.

    @headerfile lvtk/ext/worker.hpp
    @ingroup worker
*/
template <class I>
struct Worker : Extension<I> {
    /** @private */
    Worker (const FeatureList& features) {
        for (const auto& f : features)
            if (schedule_work.set (f))
                break;
    }

    /** Perform work as requested by schedule_work
     
        @param respond  Function to send responses with
     */
    WorkerStatus work (WorkerRespond& respond, uint32_t size, const void* data) { return LV2_WORKER_SUCCESS; }

    /** Process work responses sent with respond in the `work` callback
     
        @param size Size of response data
        @param data The reponse data
     */
    WorkerStatus work_response (uint32_t size, const void* body) { return LV2_WORKER_SUCCESS; }

    /** Called at the end of a processing cycle */
    WorkerStatus end_run() { return LV2_WORKER_SUCCESS; }

    /** Use this to schedule a job with the host.  This is a function object so
        it can be called directly.
        
        Example:
        @code
            void run (uint32_t nframes) {
                // ...
                
                schedule_work (strlen ("non_rt_job"), "non_rt_job"));
                
                // ...
            }
        @endcode
        The above code would result in `work` being called where "non_rt_job"
        is the data parameter
     */
    WorkerSchedule schedule_work;

protected:
    /** @private */
    static void map_extension_data (ExtensionMap& dmap) {
        static const LV2_Worker_Interface _worker = { _work, _work_response, _end_run };
        dmap[LV2_WORKER__interface] = &_worker;
    }

private:
    /** @internal */
    static LV2_Worker_Status _work (LV2_Handle instance,
                                    LV2_Worker_Respond_Function respond,
                                    LV2_Worker_Respond_Handle handle,
                                    uint32_t size,
                                    const void* data) {
        WorkerRespond wrsp (instance, respond, handle);
        return (LV2_Worker_Status) (static_cast<I*> (instance))->work (wrsp, size, data);
    }

    /** @internal */
    static LV2_Worker_Status _work_response (LV2_Handle instance,
                                             uint32_t size,
                                             const void* body) {
        return (LV2_Worker_Status) (static_cast<I*> (instance))->work_response (size, body);
    }

    /** @internal */
    static LV2_Worker_Status _end_run (LV2_Handle instance) {
        return (LV2_Worker_Status) (static_cast<I*> (instance))->end_run();
    }
};

} // namespace lvtk
