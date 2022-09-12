#pragma once

#include <vector>

namespace lvtk {
namespace demo {

template <class Wgt>
static inline void delete_widgets (std::vector<Wgt*>& vec) {
    for (auto w : vec)
        delete w;
    vec.clear();
}

} // namespace demo
} // namespace lvtk