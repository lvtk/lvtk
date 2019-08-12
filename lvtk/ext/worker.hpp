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

#pragma once

#include <lv2/worker/worker.h>

namespace lvtk {

/** Convenience enum to get LV2_Worker_Status into a C++ namespace */
typedef enum {
    WORKER_SUCCESS          = LV2_WORKER_SUCCESS,      /**< Work Completed. */
    WORKER_ERR_UNKNOWN      = LV2_WORKER_ERR_UNKNOWN,  /**< Unknown error. */
    WORKER_ERR_NO_SPACE     = LV2_WORKER_ERR_NO_SPACE  /**< Fail due to Lack of Space. */
} WorkerStatus;


/** Worker reponse. 
    This wraps an LV2_Worker_Respond_Function and exeucutes via operator ()
  */
struct WorkerRespond
{
    WorkerRespond (LV2_Handle                  instance,
                    LV2_Worker_Respond_Function wrfunc,
                    LV2_Worker_Respond_Handle   handle)
            : p_instance (instance),
              p_handle (handle), 
              p_wrfunc (wrfunc) 
    { }

    /** Execute the worker retrieval functor.
        @param size
        @param data
        @return WORKER_SUCCESS on success
     */
    WorkerStatus operator() (uint32_t size, const void* data) const
    {
        return (WorkerStatus) p_wrfunc (p_handle, size, data);
    }

private:
    LV2_Handle                        p_instance;
    LV2_Worker_Respond_Handle         p_handle;
    LV2_Worker_Respond_Function       p_wrfunc;
};

struct Scheduler : FeatureData<LV2_Worker_Schedule>
{
    Scheduler() : FeatureData<LV2_Worker_Schedule> (LV2_WORKER__schedule) {}
    
    WorkerStatus schedule_work (uint32_t size, const void* data) const {
        auto& ext = this->data;
        if (ext.schedule_work)
            return (WorkerStatus) ext.schedule_work (ext.handle, size, data);
        return WORKER_ERR_UNKNOWN; 
    }

    WorkerStatus operator() (uint32_t size, const void* buffer) const {
        return schedule_work (size, buffer);
    }
};

} /* namespace lvtk */
