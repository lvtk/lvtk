/*
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
*/

#pragma once

#include <stdio.h>
#include <lv2/log/log.h>
#include <lvtk/feature.hpp>

namespace lvtk {

struct Log
{
    void set_feature (const Feature& feature)
    {
        p_log = reinterpret_cast<LV2_Log_Log*> (feature.data);
    }
    
    int vprintf (LV2_URID type, const char* fmt, va_list ap) const
    {
        if (p_log != NULL)
            return p_log->vprintf(p_log->handle, type, fmt, ap);
        return ::vprintf (fmt, ap);
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
