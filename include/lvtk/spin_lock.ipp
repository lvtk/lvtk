// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/spin_lock.hpp>

#if _WIN32
#    define WINDOWS_LWAN
#    define WIN32_LEAN_AND_MEAN 1
#    include <windows.h>
#    undef WIN32_LEAN_AND_MEAN
#    undef min
#    undef max
#else
#    include <sched.h>
#endif

namespace lvtk {

void SpinLock::lock() const noexcept {
    if (try_lock())
        return;

    for (int i = 20; --i >= 0;)
        if (try_lock())
            return;

    while (! try_lock()) {
#if _WIN32
        Sleep (0);
#else
        sched_yield();
#endif
    }
}

} // namespace lvtk
