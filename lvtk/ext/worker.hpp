/*

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

*/

#ifndef LVTK_WORKER_HPP
#define LVTK_WORKER_HPP

#include <cstring>

#include <lv2/lv2plug.in/ns/ext/worker/worker.h>

#include "lvtk/private/types.hpp"

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

   /** The LV2 Worker Feature Mixin
       @struct lvtk::Worker lvtk/ext/worker.hpp
       @ingroup pluginmixins
   */
   template <bool Required = true>
   struct Worker
   {
        template <class Derived>
        struct I : Extension<Required>
        {

         /** @skip */
         static void
         map_feature_handlers(FeatureHandlerMap& hmap)
         {
            hmap[LV2_WORKER__schedule] = &I<Derived>::handle_feature;
         }

         /** @skip */
         static void
         handle_feature(void* instance, FeatureData data)
         {
            Derived* d = reinterpret_cast<Derived*>(instance);
            I<Derived>* fe = static_cast<I<Derived>*>(d);
            LV2_Worker_Schedule *ws = reinterpret_cast<LV2_Worker_Schedule*>(data);

            fe->m_work_schedule_handle = ws->handle;
            fe->m_schedule_work_func = ws->schedule_work;
            fe->m_ok = true;
         }

         /** Sanity check the mixin */
         bool
         check_ok()
         {
            if (LVTK_DEBUG) {
              std::clog<<"    [Worker] validation "
                       <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
            }
            return this->m_ok;
         }

         /** @skip */
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

         /* =============== LV2 Worker C++ Interface =============== */


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
            if this function returns WORKER_SUCCESS.

            @param size   The size of @p data.
            @param data   Message to pass to work(), or NULL.
         */
         WorkerStatus
         schedule_work (uint32_t size, const void* data)
         {
             return (WorkerStatus)m_schedule_work_func (m_work_schedule_handle, size, data);
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
          work (WorkerRespond &respond, uint32_t size, const void* data)
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
          work_response (uint32_t size, const void* body)
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

         /* LV2 Worker Implementation */

         /** @internal */
         LV2_Worker_Schedule_Handle m_work_schedule_handle;

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

} /* namespace lvtk */

#endif /* LVTK_WORKER_HPP */
