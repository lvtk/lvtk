
#pragma once

#if __linux__

#include <assert.h>
#include <dlfcn.h>
#include <evg/evg.h>

#include <lvtk/ui/embed.hpp>

#include "../button.hpp"
#include "display.hpp"
#include "engine.hpp"

namespace lvtk {
namespace demo {

class Video : public lvtk::Widget {
public:
    Video (Main& m) : main (m) {
        add (tgl_engine);
        tgl_engine.set_visible (true);
        tgl_engine.clicked = [this]() {
            tgl_engine.toggle (! tgl_engine.toggled());
            set_running (tgl_engine.toggled());
        };

        display.reset (new VideoDisplay (m));
        add (display.get());
        display->set_visible (true);
        set_size (360, 240);
        set_running (true);
    }

    ~Video() {
        display.reset();
        tgl_engine.clicked = nullptr;
        set_running (false);
        engine.reset();
        if (device_handle)
            dlclose (device_handle);
    }

    void resized() {
        auto r = bounds().at (0);
        display->set_bounds (r.slice_top (height() - 50).smaller (2));
        r.reduce (4);
        r.slice_right (12);
        tgl_engine.set_bounds (r.slice_right (92).smaller (0, 8));
    }

    void set_running (bool r) {
        if (r == running())
            return;
        if (r) {
            enable_engine (true);
            enable_display (true);
        } else {
            enable_display (false);
            enable_engine (false);
        }

        tgl_engine.toggle (running());
    }

    void enable_display (bool enabled) {
        if (! engine) {
            std::clog << "[video] display needs an engine...\n";
            return;
        }
        if (enabled) {
            auto swap = display->make_swap();
            if (swap.window != 0)
                engine->set_swap (&swap);
            else
                std::clog << "[video] couldn't initialize swap\n";
        } else {
            engine->set_swap (nullptr);
        }
    }

    bool running() const noexcept {
        return engine != nullptr && engine->is_running();
    }

    void enable_engine (bool enabled) {
        if (enabled) {
            if (! engine) {
                if (load_device()) {
                    engine = std::make_unique<VideoEngine>();
                    if (! engine->load_device_descriptor (&device))
                        engine.reset();
                }
            }

            if (engine)
                engine->start_thread();
        } else {
            if (engine)
                engine->stop_thread();
        }
    }

private:
    Main& main;

    Button tgl_engine,
        tgl_solid,
        tgl_image;

    std::unique_ptr<VideoDisplay> display;
    std::unique_ptr<VideoEngine> engine;
    evgDeviceDescriptor device;
    void* device_handle = nullptr;

    bool load_device() {
        if (device_handle != nullptr)
            return true;

        void* handle = nullptr;
        evgDeviceFunction* factory = nullptr;
        const evgDeviceDescriptor* desc = nullptr;
        handle = dlopen ("subprojects/evg/devices/opengl.so", RTLD_LOCAL | RTLD_LAZY);
        factory = handle != nullptr ? (evgDeviceFunction*) dlsym (handle, "evg_device_descriptor") : nullptr;
        desc = factory != nullptr ? factory() : nullptr;

        bool result = desc != nullptr && handle != nullptr;

        if (result) {
            device_handle = handle;
            device = *desc;
        } else {
            device_handle = nullptr;
            device = {};
            if (handle)
                dlclose (handle);
        }

        return result;
    }
};

} // namespace demo
} // namespace lvtk

#endif
