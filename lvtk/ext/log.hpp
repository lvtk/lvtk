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
#include <lv2/log/log.h>
#include <lvtk/lvtk.hpp>

namespace lvtk {

struct Logger
{
    void set_feature (const Feature& feature)
    {
        p_log = reinterpret_cast<LV2_Log_Log*> (feature.data);
    }
    
    int vprintf (LV2_URID type, const char* fmt, va_list ap) const
    {
        if (p_log != NULL)
            return p_log->vprintf(p_log->handle, type, fmt, ap);
        return 0;
    }

    int printf (LV2_URID type, const char* fmt, ...) const
    {
        va_list argptr;
        va_start (argptr, fmt);

        int res = this->vprintf(type, fmt, argptr);
        va_end (argptr);

        return res;
    }

private:
    LV2_Log_Log* p_log;
};

}
