
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

    bool obstructed (int x, int y) {
        return true;
    }

    void resized() override {
        std::clog << "[embedding] resized: " << bounds().at(0).str() << std::endl;
        if (embed && embed->visible())
            embed->set_bounds (bounds());
    }

    void paint (Graphics& g) override {
        g.set_color (0x222222ff);
        g.fill_rect (bounds().at (0, 0));
    }

    void unload_volume() {
        idle = [](){};
        if (embed)
            remove (*embed);
        embed.reset();

        if (ui) {
            ui->unload();
            ui.reset();
        }

        if (plugin) {
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
        
        embed.reset (new Embed (main));
        add (*embed);

        if (auto hv = embed->host_view()) {
            ui = plugin->instantiate_ui (hv->handle());
            if (ui) {
                embed->set_visible (true);
                idle = [this]() { ui->idle(); };
            }
        }

        resized();
        repaint();
    }

    bool __wants_updates() override {
        return true;
    }

    void __update() override {
        if (! loaded())
            load_volume();
        idle();
    };

protected:
    void parent_structure_changed() override {
        
    }
private:
    std::function<void()> idle { [](){} };
    lvtk::World world;
    lvtk::Main& main;
    std::unique_ptr<Embed> embed;
    std::unique_ptr<lvtk::Instance> plugin;
    std::unique_ptr<lvtk::InstanceUI> ui;
};

} // namespace demo
} // namespace lvtk
