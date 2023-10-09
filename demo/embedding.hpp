// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <iostream>

#include <lvtk/ui/embed.hpp>
#include <lvtk/ui/widget.hpp>

#include "utils.hpp"

namespace lvtk {
namespace demo {

class Embedding : public Widget {
public:
    Embedding() {
        set_size (640, 360);
        set_opaque (true);
    }

    ~Embedding() {
        conn_idle.disconnect();
        unload_volume();
    }

    bool obstructed (int x, int y) override {
        return true;
    }

    void resized() override {
        if (embed && embed->visible())
            embed->set_bounds (bounds().at (0, 0));
    }

    void paint (Graphics& g) override {
        g.set_color (0xff222222);
        g.fill_rect (bounds().at (0, 0));
    }

    void unload_volume() {
        conn_idle.disconnect();
        f_idle = []() {};
        if (embed)
            remove (*embed);
        embed.reset();
    }

    bool loaded() const {
        return embed != nullptr;
    }

    void load_volume() {
        if (loaded())
            return;

#if 0
        if (plugin == nullptr) {
            // plugin = world.instantiate (LVTK_PLUGINS__Volume);
            if (! plugin) {
                std::clog << "[demo] plugin faile to instantiate: \n"
                          << LVTK_PLUGINS__Volume << std::endl;
                return;
            }

            std::clog << "[demo] load: " << plugin->name() << std::endl;
            plugin->activate();
        }

        if (embed == nullptr) {
            embed = std::make_unique<Embed>();
            add (*embed);
        }

        auto pv = embed->proxy_view();
        if (pv != nullptr)
            if (auto handle = pv->handle())
                ui = plugin->instantiate_ui (handle);

        if (ui != nullptr && embed != nullptr) {
            std::clog << "[demo] ui created" << std::endl;
            embed->set_visible (true);
            f_idle = [this]() {
                ui->idle();
            };
        }
#endif
        resized();
        repaint();
    }

    void idle() {
        if (! loaded())
            load_volume();
        f_idle();
    };

protected:
    void parent_structure_changed() override {
        if (loaded())
            return;
        if (lvtk::ViewRef view = find_view()) {
            conn_idle.disconnect();
            conn_idle = view->connect_idle (std::bind (&Embedding::idle, this));
        }
    }

private:
    boost::signals2::connection conn_idle;
    std::function<void()> f_idle { []() {} };
    std::unique_ptr<Embed> embed;
};

} // namespace demo
} // namespace lvtk
