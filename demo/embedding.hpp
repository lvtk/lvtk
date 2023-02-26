// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <iostream>

#include <lvtk/host/world.hpp>
#include <lvtk/ui/embed.hpp>
#include <lvtk/ui/widget.hpp>

#include "utils.hpp"

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
        g.set_color (0x222222ff);
        g.fill_rect (bounds().at (0, 0));
    }

    void unload_volume() {
        conn_idle.disconnect();
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

        if (plugin == nullptr) {
            plugin = world.instantiate (LVTK_PLUGINS__Volume);
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
    lvtk::World world;
    std::unique_ptr<Embed> embed;
    std::unique_ptr<lvtk::Instance> plugin;
    std::unique_ptr<lvtk::InstanceUI> ui;
};

} // namespace demo
} // namespace lvtk
