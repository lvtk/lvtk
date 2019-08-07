/*
    lvtk.hpp - Support file for writing LV2 plugins in C++
    Copyright (C) 2013-2019  Michael Fisher <mfisher31@gmail.com>

    @@ISC@@
*/

#pragma once

#include <cstdlib>
#include <vector>

#include <lv2/core/lv2.h>
#include <lv2/urid/urid.h>
#include <lv2/worker/worker.h>

namespace lvtk {

using Feature       = LV2_Feature;
using URID          = LV2_URID;
using String        = std::string;
using StringArray   = std::vector<String>;
using ExtensionMap  = std::map<String, const void*>;

template<class T>
struct DesctriptorList final : public std::vector<T>
{
    inline ~DesctriptorList() {
        for (const auto& desc : *this)
            std::free ((void*) desc.URI);
    }
};

template<typename T>
inline void debug (const T& msg) {
    std::clog << "[lvtk] " << msg << std::endl;
}

}
