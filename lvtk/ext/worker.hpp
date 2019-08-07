/*
    This file is part of LV2 Toolkit
    Copyright (C) 2012-2019 Michael Fisher <mfisher31@gmail.com>

    @@ISC@@
 */

#pragma once

#include <cstring>
#include <lv2/worker/worker.h>

namespace lvtk {

/** Convenience enum to get LV2_Worker_Status into a C++ namespace */
typedef enum {
    WORKER_SUCCESS      = LV2_WORKER_SUCCESS,      /**< Work Completed. */
    WORKER_ERR_UNKNOWN  = LV2_WORKER_ERR_UNKNOWN,  /**< Unknown error. */
    WORKER_ERR_NO_SPACE = LV2_WORKER_ERR_NO_SPACE  /**< Fail due to Lack of Space. */
} WorkerStatus;


/**
    Worker reponse. This wraps an LV2_Worker_Respond_Function
     and exeucutes via operator ()
   */
struct WorkerRespond {
    WorkerRespond(LV2_Handle instance,
                  LV2_Worker_Respond_Function wrfunc,
                  LV2_Worker_Respond_Handle handle)
         : p_instance(instance),
         p_handle(handle), 
         p_wrfunc (wrfunc) 
   { }

   /**
       Execute the worker retrieval functor.
         @param size
         @param data
         @return WORKER_SUCCESS on success
      */
   WorkerStatus operator () (uint32_t size, const void* data) const
   {
      return (WorkerStatus) p_wrfunc (p_handle, size, data);
   }

private:
    LV2_Handle                        p_instance;
    LV2_Worker_Respond_Handle         p_handle;
    LV2_Worker_Respond_Function       p_wrfunc;
};

template<class InstanceType>
struct Worker
{
   Worker()
   {
      static const LV2_Worker_Interface worker = 
         { _work, _work_response, _end_run };
      
      Plugin<InstanceType>::register_extension (
         LV2_WORKER__interface, &worker );
   }

   /** @internal */
   static LV2_Worker_Status _work (LV2_Handle               instance,
                                    LV2_Worker_Respond_Function respond,
                                    LV2_Worker_Respond_Handle   handle,
                                    uint32_t                    size,
                                    const void*                 data)
   {
      InstanceType* plugin = reinterpret_cast<InstanceType*>(instance);
      WorkerRespond wrsp (instance, respond, handle);
      return (LV2_Worker_Status)plugin->work(wrsp, size, data);
   }

   /** @internal */
   static LV2_Worker_Status _work_response (LV2_Handle  instance,
                                             uint32_t    size,
                                             const void* body)
   {
      InstanceType* plugin = reinterpret_cast<InstanceType*>(instance);
      return (LV2_Worker_Status)plugin->work_response (size, body);
   }

   /** @internal */
   static LV2_Worker_Status _end_run (LV2_Handle instance)
   {
      InstanceType* plugin = reinterpret_cast<InstanceType*>(instance);
      return (LV2_Worker_Status)plugin->end_run();
   }
};

struct WorkerSchedule 
{
    WorkerStatus operator() (uint32_t size, const void* data) const {
      if (worker.schedule_work)
         return (WorkerStatus) worker.schedule_work (worker.handle, size, data);
      return WORKER_ERR_UNKNOWN; 
   }

   void set_feature (const Feature& feature) {
      worker = *(LV2_Worker_Schedule*) feature.data;
   }

private:
   LV2_Worker_Schedule worker;
};

} /* namespace lvtk */
