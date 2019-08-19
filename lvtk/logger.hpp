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

#include <stdio.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lvtk/map.hpp>

namespace lvtk {

/** Wrapper around LV2_Log_Log
    
    Use this on the stack to log messages
    @headerfile lvtk/ext/log.hpp
    @ingroup log
 */
struct Logger : FeatureData<LV2_Log_Log> {
    /** ctor */
    Logger() : FeatureData<LV2_Log_Log> (LV2_LOG__log) {}

    /** Log a log:Trace message with stream operator */
    inline void operator<< (const std::string& out) const { operator<< (out.c_str()); }

    /** Log log:Trace with stream operator */
    inline void operator<< (const char* out) const { 
        if (log_Trace > 0)
            this->printf (log_Trace, out);
    }
    
    /** Log message with va_list 
     
        @param type     LV2_URID type to log
        @fmt            Format / message
        @ap             Arguments
    */
    inline int vprintf (uint32_t type, const char* fmt, va_list ap) const
    {
        if (data.handle != nullptr)
            return data.vprintf (data.handle, type, fmt, ap);
        return 0;
    }

    /** Log message with var args
     
        @param type     LV2_URID type to log
        @fmt            Format / message
    */
    inline int printf (uint32_t type, const char* fmt, ...) const
    {
        va_list argptr;
        va_start (argptr, fmt);
        int res = this->vprintf (type, fmt, argptr);
        va_end (argptr);

        return res;
    }

    /** Assign LV2_URIDs needed to log messages
       
        @param map  A LV2_URID_Map to inititialize with
     */
    inline void init (LV2_URID_Map* const map)
    {
        log_Entry   = map->map (map->handle, LV2_LOG__Entry);
        log_Error   = map->map (map->handle, LV2_LOG__Error);
        log_Note    = map->map (map->handle, LV2_LOG__Note);
        log_Trace   = map->map (map->handle, LV2_LOG__Trace);
        log_Warning = map->map (map->handle, LV2_LOG__Warning);
    }

private:
    uint32_t log_Entry = 0;
    uint32_t log_Error = 0;
    uint32_t log_Note  = 0;
    uint32_t log_Trace = 0;
    uint32_t log_Warning = 0;
};

}
