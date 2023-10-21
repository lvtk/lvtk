// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <atomic>

#include <lvtk/lvtk.hpp>

namespace lvtk {

/** A simple spin lock using std::atomic.
    
    In order to use this as header-only, you also must include 
    <lvtk/spin_lock.ipp> in an implementation file to compile platform 
    specific code.
 */
class LVTK_API SpinLock {
public:
    /** Initialize unlocked. */
    inline SpinLock() = default;
    inline ~SpinLock() = default;

    /** Lock or yield until locked. (non realtime) */
    void lock() const noexcept;
    /** Lock immediately or return false. (realtime)*/
    inline bool try_lock() const noexcept { return try_lock (0, 1); }
    /** Unlock the mutex. Note this does not check lock status. */
    inline void unlock() const noexcept { _lock = 0; }
    /** Returns true if the mutex is locked. */
    inline bool locked() const noexcept { return _lock.load() == 1; }

private:
    mutable std::atomic<int> _lock {0};
    /** @internal */
    inline bool try_lock (int c, int v) const noexcept {
        return _lock.compare_exchange_strong (c, v);
    }

    LVTK_DISABLE_COPY(SpinLock)
};

} // namespace lvtk
