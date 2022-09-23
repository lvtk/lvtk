// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ext/extension.hpp>

#include <lv2/worker/worker.h>

namespace lvtk {

/** Alias of LV2_Worker_Status
    @ingroup alias
    @headerfile lvtk/ext/worker.hpp
*/
using WorkerStatus = LV2_Worker_Status;

/** Worker reponse function

    This wraps an LV2_Worker_Respond_Function.  It is passed to
    the work method on your Instance

    @ingroup utility
    @headerfile lvtk/ext/worker.hpp
 */
struct WorkerRespond {
    WorkerRespond (LV2_Handle instance,
                   LV2_Worker_Respond_Function respond_function,
                   LV2_Worker_Respond_Handle handle)
        : p_handle (handle),
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
    LV2_Worker_Respond_Handle p_handle;
    LV2_Worker_Respond_Function f_respond;
};

/** Schedule jobs with the host.

    This wraps LV2_Worker_Schedule.  Used by the Worker interface to add
    `schedule_work` to a plugin instance.  You shouldn't need to use it
    directly. @see @ref Worker

    @ingroup utility
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
    @ingroup ext
*/
template <class I>
struct Worker : Extension<I> {
    /** @private */
    Worker (const FeatureList& features) {
        for (const auto& f : features)
            if (_schedule.set (f))
                break;
    }

    /** Perform work as requested by schedule_work

        @param respond  Function to send responses with
        @param size     The size of data
        @param data     The data
     */
    WorkerStatus work (WorkerRespond& respond, uint32_t size, const void* data) { return LV2_WORKER_SUCCESS; }

    /** Process work responses sent with respond in the `work` callback

        @param size Size of response data
        @param data The reponse data
     */
    WorkerStatus work_response (uint32_t size, const void* data) { return LV2_WORKER_SUCCESS; }

    /** Called at the end of a processing cycle

        This callback is optional.
        @returns Return a status of success or not
    */
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
    WorkerStatus schedule_work (size_t size, void* data) const noexcept {
        return _schedule (size, data);
    }

    /** Get the Worker Schedule object 
        @returns the WorkerSchedule
     */
    WorkerSchedule& schedule() { return _schedule; }

protected:
    /** @private */
    static void map_extension_data (ExtensionMap& dmap) {
        static const LV2_Worker_Interface _worker = { _work, _work_response, _end_run };
        dmap[LV2_WORKER__interface]               = &_worker;
    }

private:
    WorkerSchedule _schedule;

    /** @private */
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
