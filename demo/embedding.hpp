
#pragma once

#include "utils.hpp"
#include <lvtk/ui/widget.hpp>
#include <lvtk/ui/embed.hpp>
#include <lvtk/host/world.hpp>

namespace lvtk {
namespace demo {

class Embedding : public Widget {
public:
    Embedding() = delete;
    Embedding (Main& m) :main(m) {
        set_size (640, 360);
        world.load_all();
        set_opaque (true);
    }

    ~Embedding() {
        unload_volume();
    }

    void resized() override {
        if (embed)
            embed->set_bounds (bounds().at(0).smaller(10));
    }

    void paint (Graphics& g) override {
        g.set_color (0x222222ff);
        g.fill_rect (bounds().at (0, 0));
    }

    void unload_volume() {
        embed.reset();

        if (ui) {
            ui->unload();
            ui.reset();
        }

        if (plugin) {
            plugin.reset();
        }
    }

    void load_volume() {
        unload_volume();

        plugin = world.instantiate (LVTK_PLUGINS__Volume);
        if (! plugin) {
            return;
        }
        embed.reset (new Embed (main));
        if (auto hv = embed->host_view()) {
            ui = plugin->instantiate_ui (hv->handle());
            add (*embed);
            embed->set_visible (true);
        }

        resized();
        repaint();
    }

protected:
    void parent_structure_changed() override {
        load_volume();
    }
private:
    lvtk::World world;
    lvtk::Main& main;
    std::unique_ptr<Embed> embed;
    std::unique_ptr<lvtk::Instance> plugin;
    std::unique_ptr<lvtk::InstanceUI> ui;
};

} // namespace demo
} // namespace lvtk
