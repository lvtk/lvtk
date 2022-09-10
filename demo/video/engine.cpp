
#include <cassert>
#include <cstring>
#include <iostream>

#include <evg/context.hpp>
#include <lvtk/ui/widget.hpp>

#include "engine.hpp"
#include "video/image_source.hpp"
#include "video/solid_source.hpp"

namespace lvtk {
namespace demo {

template <uint32_t R>
struct fps_to_nanoseconds {
    static constexpr const auto value = std::chrono::nanoseconds (1000000000 / R);
};

using FPS30 = fps_to_nanoseconds<30>;
using FPS60 = fps_to_nanoseconds<60>;
using FPS24 = fps_to_nanoseconds<24>;
using FPS5 = fps_to_nanoseconds<5>;

//=============================================================================
class Screen {
public:
    Screen (ImageSource& s, SolidSource& sd, evg::Swap* sw)
        : source (s), solid (sd), swap (sw) {}

    void render (evg::Context& g) {
        auto& device = g.get_device();
        device.load_swap (swap.get());
        device.enter_context();

        device.viewport (0, 0, 640, 360);
        device.clear (EVG_CLEAR_COLOR, 0xff050505, 0.0, 0);
        g.ortho (0.0f, 640.0, 0.0f, 360.0, -100.0, 100.0);

        g.save_state();
        solid.expose (g);
        g.restore_state();

        g.save_state();
        source.expose (g);
        g.restore_state();

        device.flush();
        device.present();

        device.load_swap (nullptr);
        device.leave_context();
    }

private:
    evg::Source& source;
    evg::Source& solid;
    std::unique_ptr<evg::Swap> swap;
};

//=============================================================================
static std::unique_ptr<Screen> screen;
static std::unique_ptr<ImageSource> source;
static std::unique_ptr<SolidSource> solid;
static std::unique_ptr<evg::Context> context;
static std::mutex screen_mutex;

static bool process_video (VideoEngine& video) {
    if (! context) {
        context.reset (new evg::Context (video.graphics_device()));
    }

    if (context == nullptr || source == nullptr)
        return false;

    const auto stop_requested = video.should_stop();
    const auto interval = fps_to_nanoseconds<24>::value;
    auto& device = video.graphics_device();

    {
        device.enter_context();
        std::scoped_lock sl1 (solid->render_mutex());
        solid->process_frame();
        std::scoped_lock sl2 (source->render_mutex());
        source->process_frame();
        device.leave_context();

        {
            std::scoped_lock sl2 (screen_mutex);
            if (screen)
                screen->render (*context);
        }
    }

    std::this_thread::sleep_for (interval);
    return ! stop_requested;
}

void VideoEngine::thread_entry (VideoEngine& video) {
    while (process_video (video))
        ;
    screen.reset();
    source.reset();
    solid.reset();
    context.reset();
    std::clog << "[video] thread stopped\n";
    video.stopflag.store (0);
    video.running.store (0);
}

VideoEngine::VideoEngine() {}

VideoEngine::~VideoEngine() {
    if (is_running())
        stop_thread();

    if (graphics) {
        // graphics->unload();
        graphics.reset();
    }
}

void VideoEngine::set_swap (const evgSwapInfo* setup) {
    if (graphics == nullptr) {
        std::clog << "[video] swap: no graphics to render with...\n";
        return;
    }

    if (source == nullptr) {
        solid.reset (new SolidSource());
        source.reset (new ImageSource());
        // source->load_file ("/home/mfisher/Desktop/500_x_500_SMPTE_Color_Bars.png");
    }

    if (setup == nullptr) {
        std::clog << "[video] releasing swap...\n";
        std::unique_ptr<Screen> deleter;
        {
            std::lock_guard<std::mutex> sl (screen_mutex);
            screen.swap (deleter);
        }
        deleter.reset();
        return;
    }

    auto swap = graphics->create_swap (setup);
    if (swap == nullptr) {
        std::clog << "[video] failed to load swap...\n";
        return;
    }

    auto screen2 = std::make_unique<Screen> (*source, *solid, swap);
    {
        std::clog << "[video] loading swap...\n";
        std::lock_guard<std::mutex> sl (screen_mutex);
        screen.swap (screen2);
    }

    screen2.reset();
}

bool VideoEngine::load_device_descriptor (const evgDeviceDescriptor* desc) {
    if (graphics)
        return true;

    if (auto g = evg::Device::open (desc)) {
        graphics = std::move (g);
        // FIXME: don't do this here.
        // if (! is_running())
        //     start_thread();
    }

    return graphics != nullptr;
}

void VideoEngine::start_thread() {
    if (! graphics) {
        std::cerr << "[video] cannot start video engine without a graphics device.\n";
        return;
    }

    if (is_running())
        return;
    std::clog << "[video] engine starting\n";
    stopflag.store (0);
    running.store (1);
    video_thread = std::thread (VideoEngine::thread_entry, std::ref (*this));
}

void VideoEngine::stop_thread() {
    if (is_running()) {
        stopflag.store (1);

        int retries = 4;
        while (is_running() && --retries >= 0) {
            std::this_thread::sleep_for (std::chrono::milliseconds (14));
        }

        stopflag.store (0);
        running.store (0);
    }

    if (video_thread.joinable())
        video_thread.join();
}

} // namespace demo
} // namespace lvtk
