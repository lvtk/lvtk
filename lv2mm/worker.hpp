/****************************************************************************

    worker.hpp - Support file for writing LV2 plugins in C++

    Copyright (C) 2012 Michael Fisher <mfisher31@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef LV2_WORKER_HPP
#define LV2_WORKER_HPP

#include <lv2/lv2plug.in/ns/ext/worker/worker.h>

namespace LV2 {

   /** Convenience enum to get LV2_Worker_Status into a C++ namespace */
   typedef enum {
      WORKER_SUCCESS      = LV2_WORKER_SUCCESS,      /**< Work Completed. */
      WORKER_ERR_UNKNOWN  = LV2_WORKER_ERR_UNKNOWN,  /**< Unknown error. */
      WORKER_ERR_NO_SPACE = LV2_WORKER_ERR_NO_SPACE  /**< Fail due to Lack of Space. */
   } WorkerStatus;

   typedef LV2_Worker_Respond_Handle    WorkerRespondHandle;
   typedef LV2_Worker_Respond_Function  WorkerRespondFunction;
   typedef LV2_Worker_Schedule          WorkerSchedule;

   /**
       Wrapper struct for state retrieval. This wraps an
       LV2_State_Retrieve_Function and exeucutes via operator ()
    */
   struct WorkerRespond {
      WorkerRespond(LV2_Handle instance,
                    WorkerRespondFunction wrfunc,
                    WorkerRespondHandle handle)
      : p_instance(instance), p_wrfunc(wrfunc), p_handle(handle) { }

      /**
          Execute the retrieve functor.
          @param key
          @param size
          @param type
          @param flags
          @return Associate 'value' data for the given key
       */
      WorkerStatus operator () (uint32_t size, const void* data)
      {
         return (WorkerStatus) p_wrfunc (p_handle, size, data);
      }

   private:
      LV2_Handle                p_instance;
      WorkerRespondHandle       p_handle;
      WorkerRespondFunction     p_wrfunc;
   };

   /** The LV2 Worker Feature.
       The actual type that your plugin class will inherit when you use
       this mixin is the internal struct template I.
       @ingroup pluginmixins
    */
   LV2MM_MIXIN_CLASS Worker {

      /** This is the type that your plugin class will inherit when you use the
          Worker mixin. The public and protected members defined here
          will be available in your plugin class.
      */
      LV2MM_MIXIN_DERIVED {

         /** @internal */
         static void
         map_feature_handlers(FeatureHandlerMap& hmap)
         {
            hmap[LV2_WORKER__schedule] = &I<Derived>::handle_feature;
         }

         /** @internal */
         static void
         handle_feature(LV2::Handle instance, FeatureData data)
         {
            Derived* d = reinterpret_cast<Derived*>(instance);
            I<Derived>* fe = static_cast<I<Derived>*>(d);

            if (LV2MM_DEBUG) {
               std::clog<<"  [LV2::Worker] handle_feature\n";
            }

            WorkerSchedule *ws = reinterpret_cast<WorkerSchedule*>(data);
            fe->m_worker_schedule_handle = ws->handle;
            fe->m_schedule_work_func = ws->schedule_work;
            fe->m_ok = true;
         }

         /** @internal */
         bool
         check_ok()
         {
            if (LV2MM_DEBUG) {
              std::clog<<"    [LV2::Worker] Validation "
                       <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
            }
            return this->m_ok;
         }

         /** @internal */
         static const void*
         extension_data (const char* uri)
         {
            if (!std::strcmp (uri, LV2_WORKER__interface)) {
               static LV2_Worker_Interface worker = { &I<Derived>::_work,
                                                      &I<Derived>::_work_response,
                                                      &I<Derived>::_end_run };
               return &worker;
            }

            return 0;
         }

         /**
            Request from run() that the host call the worker.

            This function is in the audio threading class.  It should be called from
            run() to request that the host call the work() method in a non-realtime
            context with the given arguments.

            This function is always safe to call from run(), but it is not
            guaranteed that the worker is actually called from a different thread.
            In particular, when free-wheeling (e.g. for offline rendering), the
            worker may be executed immediately.  This allows single-threaded
            processing with sample accuracy and avoids timing problems when run() is
            executing much faster or slower than real-time.

            Plugins SHOULD be written in such a way that if the worker runs
            immediately, and responses from the worker are delivered immediately,
            the effect of the work takes place immediately with sample accuracy.

            The @p data MUST be safe for the host to copy and later pass to work(),
            and the host MUST guarantee that it will be eventually passed to work()
            if this function returns LV2::WORKER_SUCCESS.

            @param size   The size of @p data.
            @param data   Message to pass to work(), or NULL.
         */
          WorkerStatus
          schedule_work (uint32_t size, void *data)
          {
            return (WorkerStatus)m_schedule_work_func(
                                    m_worker_schedule_handle, size, data);
          }

         /**
            This is called by the host in a non-realtime context as requested,
            possibly with an arbitrary message to handle. A response can be sent
            to run() using @p respond.  The plugin MUST NOT make any assumptions
            about which thread calls this method, other than the fact that there
            are no real-time requirements.

            @param respond  A functor for sending a response to run().
            @param size     The size of @p data.
            @param data     Data from run(), or NULL.
         */
          WorkerStatus
          work(WorkerRespond        &respond,
                            uint32_t             size,
                            const void*          data)
          {
             return WORKER_SUCCESS;
          }


          /**
              Handle a response from the worker.  This is called by the host in the
              run() context when a response from the worker is ready.

              @param size     The size of @p body.
              @param body     Message body, or NULL.
           */
          WorkerStatus
          work_response
          (uint32_t size, const void* body)
          {
             return WORKER_SUCCESS;
          }


         /**
            Called when all responses for this cycle have been delivered.

            Since work_response() may be called after run() finished, this provides
            a hook for code that must run after the cycle is completed.

            This field may be NULL if the plugin has no use for it.  Otherwise, the
            host MUST call it after every run(), regardless of whether or not any
            responses were sent that cycle.
         */
          WorkerStatus
          end_run()
          {
             return WORKER_SUCCESS;
          }

        protected:

         /* LV2 Worker C API Implementation */

         /** @internal */
         LV2_Worker_Schedule_Handle m_worker_schedule_handle;

         /** @internal */
         LV2_Worker_Status (*m_schedule_work_func)(LV2_Worker_Schedule_Handle handle,
                                                   uint32_t                   size,
                                                   const void*                data);

         /** @internal */
         static LV2_Worker_Status _work(LV2_Handle               instance,
                                        LV2_Worker_Respond_Function respond,
                                        LV2_Worker_Respond_Handle   handle,
                                        uint32_t                    size,
                                        const void*                 data)
         {
            Derived* plugin = reinterpret_cast<Derived*>(instance);
            WorkerRespond wrsp (instance, respond, handle);
            return (LV2_Worker_Status)plugin->work(wrsp, size, data);
         }

         /** @internal */
         static LV2_Worker_Status _work_response (LV2_Handle  instance,
                                                  uint32_t    size,
                                                  const void* body)
         {
            Derived* plugin = reinterpret_cast<Derived*>(instance);
            return (LV2_Worker_Status)plugin->work_response (size, body);
         }

         /** @internal */
         static LV2_Worker_Status _end_run (LV2_Handle instance)
         {
            Derived* plugin = reinterpret_cast<Derived*>(instance);
            return (LV2_Worker_Status)plugin->end_run();
         }
      };
   };
} /* namespace LV2 */

#endif /* LV2_WORKER_HPP */
