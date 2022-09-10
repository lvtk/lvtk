/**
    This file is part of Element.
    - modified for lvtk demo
    Copyright (C) 2016-2021  Kushview, LLC.  All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
**/

#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <evg/device.hpp>

namespace lvtk {
namespace demo {

struct VideoSetup {
    int width = 0, height = 0;
};

class VideoEngine final {
public:
    VideoEngine();
    ~VideoEngine();

    void set_swap (const evgSwapInfo* setup);
    bool load_device_descriptor (const evgDeviceDescriptor* desc);
    evg::Device& graphics_device() { return *graphics; }

    bool is_running() const noexcept { return running.load() == 1; }
    bool should_stop() const noexcept { return stopflag.load() == 1; }

    void start_thread();
    void stop_thread();

private:
    std::unique_ptr<evg::Device> graphics;
    std::mutex compositor_lock;
    std::thread video_thread;
    std::atomic<int> stopflag { 0 }, running { 0 };

    static void thread_entry (VideoEngine& video);
};

} // namespace demo
} // namespace lvtk
