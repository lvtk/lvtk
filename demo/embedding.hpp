// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "utils.hpp"
#include <lvtk/host/world.hpp>
#include <lvtk/ui/embed.hpp>
#include <lvtk/ui/widget.hpp>

namespace lvtk {
namespace demo {

class Embedding : public Widget {
public:
    Embedding() {
        set_size (640, 360);
        world.load_all();
        set_opaque (true);
    }

    ~Embedding() {
        unload_volume();
    }

    bool obstructed (int x, int y) override {
        return true;
    }

    void resized() override {
        if (embed && embed->visible())
            embed->set_bounds (bounds());
    }

    void paint (Graphics& g) override {
        g.set_color (0x222222ff);
        g.fill_rect (bounds().at (0, 0));
    }

    void unload_volume() {
        f_idle = []() {};
        if (embed)
            remove (*embed);
        embed.reset();

        if (ui) {
            ui->unload();
            ui.reset();
        }

        if (plugin) {
            std::clog << "[demo] unload: " << plugin->name() << std::endl;
            plugin->deactivate();
            plugin.reset();
        }
    }

    bool loaded() const {
        return ui != nullptr && embed != nullptr;
    }

    void load_volume() {
        if (loaded())
            return;

        plugin = world.instantiate (LVTK_PLUGINS__Volume);
        if (! plugin) {
            return;
        }

        std::clog << "[demo] load: " << plugin->name() << std::endl;
        plugin->activate();
        embed = std::make_unique<Embed>();
        add (*embed);

        if (auto hv = embed->proxy_view()) {
            ui = plugin->instantiate_ui (hv->handle());
            if (ui) {
                embed->set_visible (true);
                f_idle = [this]() { ui->idle(); };
            }
        }

        resized();
        repaint();
    }

    void idle() {
        if (! loaded())
            load_volume();
        f_idle();
    };

private:
    std::function<void()> f_idle { []() {} };
    lvtk::World world;
    std::unique_ptr<Embed> embed;
    std::unique_ptr<lvtk::Instance> plugin;
    std::unique_ptr<lvtk::InstanceUI> ui;
};

} // namespace demo
} // namespace lvtk
