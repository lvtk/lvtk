// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <algorithm>
#include <iostream>

#include <lvtk/ext/idle.hpp>
#include <lvtk/ext/parent.hpp>
#include <lvtk/ext/resize.hpp>
#include <lvtk/lvtk.h>
#include <lvtk/options.hpp>
#include <lvtk/ui.hpp>

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/opengl.hpp>
#include <lvtk/ui/widget.hpp>

#include <lvtk/ext/urid.hpp>
#include <lvtk/weak_ref.hpp>

using namespace lvtk;

class VolumeUI final : public UI<VolumeUI, Parent, Idle, URID, Options> {
public:
    VolumeUI (const UIArgs& args)
        : UI (args) {
        for (const auto& opt : OptionArray (options())) {
            if (opt.key == map_uri (LV2_UI__scaleFactor))
                m_scale_factor = *(float*) opt.value;
        }
    }

    void cleanup() {
        content.reset();
    }

    int idle() {
        _main.loop (0);
        return 0;
    }

    void port_event (uint32_t port, uint32_t size,
                     uint32_t format, const void* buffer) {}

    LV2UI_Widget widget() {
        if (content == nullptr) {
            content = std::make_unique<Content>();
            content->set_size (360, 240);
            content->set_visible (true);
            _main.elevate (*content, 0, (uintptr_t) parent.get());
        }

        return *content;
    }

private:
    lvtk::Main _main { lvtk::Mode::MODULE, std::make_unique<lvtk::OpenGL>() };

    class Button : public Widget {
    public:
        Button()          = default;
        virtual ~Button() = default;

        void paint (Graphics& g) override {
            g.set_color (color);
            g.fill_rect (bounds().at (0, 0).as<float>());
        }

        bool obstructed (int x, int y) override {
            return true;
        }

        void pressed (const Event& ev) override {
            std::clog << name() << "   down: "
                      << ev.pos.str() << " bounds: "
                      << bounds().str() << std::endl;
        }

        void released (const Event& ev) override {
            std::clog << name() << "     up: "
                      << ev.pos.str() << std::endl;
        }

        Color color { 0xff0000ff };
    };

    class Container : public Widget {
    public:
        Container() {
            add (button1);
            button1.set_visible (true);
            button1.set_name ("button1");
            add (button2);
            button2.set_name ("button2");
            button2.set_visible (true);
        }

        void resized() override {
            auto r1 = bounds().at (0, 0);
            // std::clog << "container resized: " << r1.str() << std::endl;
            r1.width /= 2;
            auto r2           = r1;
            r2.x              = r1.width;
            const int padding = 12;
            r1.x += padding;
            r1.y += padding;
            r1.width -= (padding * 2);
            r1.height -= (padding * 2);
            r2.x += padding;
            r2.y += padding;
            r2.width -= (padding * 2);
            r2.height -= (padding * 2);

            button1.set_bounds (r1);
            button2.set_bounds (r2);
        }

        void paint (Graphics& g) override {
            g.set_color (0x777777FF);
            g.fill_rect (bounds().at (0, 0).as<float>());
        }

        Button button1, button2;
    };

    class Content : public Widget {
    public:
        Content() {
            set_name ("lvtk::VolumeUI::Content");
            add (buttons);
            buttons.set_visible (true);
            set_size (360, 240);
            set_visible (true);

            // std::clog << "created VolumeUI content\n";
        }

        ~Content() {
        }

        void motion (const Event& ev) override {
            // std::clog << "volume content motion\n";
        }

        void resized() override {
            auto r = bounds().at (0, 0);
            r.x    = 20;
            r.y    = 20;
            r.width -= (r.x * 2);
            r.height -= (r.y * 2);
            buttons.set_bounds (r);
        }

        void paint (Graphics& g) override {
            g.set_color (Color (0x545454ff));
            g.fill_rect (bounds().at (0, 0).as<float>());
        }

    private:
        Container buttons;
    };
    std::unique_ptr<Widget> content;

    float m_scale_factor { 1.f };
};

static UIDescriptor<VolumeUI> sVolumeUI (
    LVTK_PLUGINS__VolumeUI, {});
