/****************************************************************************

 log.hpp - Support file for writing LV2 plugins in C++

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

#ifndef LVTK_LV2_LOG_HPP
#define LVTK_LV2_LOG_HPP

#include <stdio.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>

namespace lvtk
{

    /** The Log Mixin
        @ingroup pluginmixins
        @headerfile lvtk/ext/log.hpp
        @see The internal struct I for API details.
     */

    template<bool Required = true>
    struct Log
    {
        template<class Derived>
        struct I : Extension<Required>
        {
            /** @skip */
            I() : p_log(NULL) { }

            /** @skip */
            static void
            map_feature_handlers(feature_handler_map& hmap)
            {
                hmap[LV2_LOG__log] = &I<Derived>::handle_feature;
            }

            /** @skip */
            static void
            handle_feature(void* instance, FeatureData data)
            {
                Derived* d = reinterpret_cast<Derived*>(instance);
                I<Derived>* mixin = static_cast<I<Derived>*>(d);

                mixin->p_log = reinterpret_cast<LV2_Log_Log*>(data);
                mixin->m_ok = true;
            }

            /** Sanity check the mixin */
            bool
            check_ok()
            {
                if (LVTK_DEBUG)
                {
                    std::clog << "    [Log] Validation "
                            << (this->m_ok ? "succeeded" : "failed")
                            << "." << std::endl;
                }
                return this->m_ok;
            }

        protected:

            /** Log a message, passing format parameters in a va_list.

                The API of this function matches that of the standard C vprintf
                function, except for the addition of the first parameter.  This
                function may be called from any non-realtime context, or from any
                context if @p type is @ref LV2_LOG__Trace.
             */
            int
            vprintf(LV2_URID type, const char* fmt, va_list ap)
            {
                if (p_log != NULL)
                    return p_log->vprintf(p_log->handle, type, fmt, ap);
                return ::vprintf (fmt, ap);
            }

            /** Log a message, passing format parameters directly.

                The API of this function matches that of the standard C printf function,
                except for the addition of the first parameter.  This function may
                be called from any non-realtime context, or from any context if @p type
                is @ref LV2_LOG__Trace.
             */
            int
            printf(LV2_URID type, const char* fmt, ...)
            {
                /* The c++ verison of log's printf, simple
                   makes a va_list and calls log's vprintf.  */
                int res = 0;
                va_list argptr;
                va_start(argptr, fmt);

                res = this->vprintf(type, fmt, argptr);

                va_end(argptr);

                return res;
            }

            LV2_Log_Log * p_log;

        };
    };
}

#endif /* LVTK_LV2_LOG_HPP */
