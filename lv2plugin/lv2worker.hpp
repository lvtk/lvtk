/****************************************************************************

    lv2worker.hpp - support file for writing LV2 plugins in C++

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

  /**
   * The Worker Feature.
   * The actual type that your plugin class will inherit when you use
   * this mixin is the internal struct template I.
   * @ingroup pluginmixins
   */
   template <bool Required = true>
   struct Worker {

     /** This is the type that your plugin class will inherit when you use the
         EventRef mixin. The public and protected members defined here
         will be available in your plugin class.
     */
     template <class Derived> struct I : Extension<Required> {

       /** @internal */
       I() { }

       /** @internal */
       static void map_feature_handlers(FeatureHandlerMap& hmap) {
         hmap[LV2_WORKER_URI] = &I<Derived>::handle_feature;
       }

       /** @internal */
       static void handle_feature(void* instance, void* data) {
         Derived* d = reinterpret_cast<Derived*>(instance);
         I<Derived>* fe = static_cast<I<Derived>*>(d);

         if (!data) { fe->m_ok = false; return; }

         LV2_Worker_Schedule *ws = reinterpret_cast<LV2_Worker_Schedule*>(data);
         fe->m_worker_schedule_handle = ws->handle;
         fe->m_schedule_work_func = ws->schedule_work;
         fe->m_ok = true;
       }

       bool check_ok() {
         if (LV2CXX_DEBUG) {
           std::clog<<"    [LV2::Worker] Validation "
                    <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
         }
         return this->m_ok;
       }

       /** @internal */
       static const void* extension_data (const char* uri) {
         if (!std::strcmp (uri, LV2_WORKER_URI)) {
           static LV2_Worker_Interface worker = { &I<Derived>::_work,
                                                  &I<Derived>::_work_response,
                                                  &I<Derived>::_end_run };
           return &worker;
         }
         return 0;
       }


       LV2_Worker_Status work(LV2_Worker_Respond_Function respond,
                              LV2_Worker_Respond_Handle   handle,
                              uint32_t                    size,
                              const void*                 data)
       { return LV2_WORKER_SUCCESS; }



       LV2_Worker_Status work_response (uint32_t    size,
                                        const void* body)
       { return LV2_WORKER_SUCCESS; }



       LV2_Worker_Status end_run () { return LV2_WORKER_SUCCESS; }

     protected:

       LV2_Worker_Schedule_Handle m_worker_schedule_handle;
       LV2_Worker_Status (*m_schedule_work_func)(LV2_Worker_Schedule_Handle handle,
                                            uint32_t                   size,
                                            const void*                data);

       static LV2_Worker_Status _work(LV2_Handle                  instance,
                                      LV2_Worker_Respond_Function respond,
                                      LV2_Worker_Respond_Handle   handle,
                                      uint32_t                    size,
                                      const void*                 data)
       {
         return reinterpret_cast<Derived*>(instance)->work (respond,handle,size,data);
       }

       static LV2_Worker_Status _work_response (LV2_Handle  instance,
                                                uint32_t    size,
                                                const void* body)
       {
         return reinterpret_cast<Derived*>(instance)->work_response (size, body);
       }

       static LV2_Worker_Status _end_run (LV2_Handle instance)
       {
         return reinterpret_cast<Derived*>(instance)->end_run ();
       }

     };
   };

}

#endif /* LV2_WORKER_HPP */
