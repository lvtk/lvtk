// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <vector>

namespace lvtk {
namespace demo {

template <class Wgt>
static inline void delete_widgets (std::vector<Wgt*>& vec) {
    for (auto w : vec)
        delete w;
    vec.clear();
    vec.shrink_to_fit();
}

} // namespace demo
} // namespace lvtk
