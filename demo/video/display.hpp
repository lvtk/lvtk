#pragma once

#include <evg/evg.h>
#include <lvtk/ui/widget.hpp>

namespace lvtk {
namespace demo {

//===========================================================================
class VideoDisplay : public lvtk::Widget {
public:
    VideoDisplay (Main& m) {
        embed.reset (new lvtk::Embed (m));
        add (*embed);
        embed->set_size (100, 100);
        set_size (640, 360);
    }

    void resized() {
        auto eb = embed->bounds();
        embed->set_bounds ((width() / 2) - (eb.width / 2),
                           (height() / 2) - (eb.height / 2),
                           eb.width,
                           eb.height);
        std::clog << "[embed] " << embed->bounds().str() << std::endl;
    }

    void paint (lvtk::Graphics& g) {
        g.set_color (0x222222ff);
        g.fill_rect (bounds().at (0));
    }

    void set_display_enabled (bool enabled) {
        remove (embed.get());

        if (enabled)
            add (embed.get());

        embed->set_visible (enabled);

        resized();
        repaint();
    }

    evgSwapInfo make_swap() const {
        evgSwapInfo setup;
        if (auto view = embed->host_view()) {
            setup.adapter = 0;
            setup.width = 640;
            setup.height = 360;
            setup.nbuffers = 1;
            setup.window.xwindow = view->handle();
            setup.format = EVG_COLOR_FORMAT_RGBA;
            setup.stencil = EVG_STENCIL_24_S8;
        }
        return setup;
    }

private:
    std::unique_ptr<lvtk::Embed> embed;
};

} // namespace demo
} // namespace lvtk
