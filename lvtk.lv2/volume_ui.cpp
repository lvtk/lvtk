
#include <algorithm>
#include <iostream>

#include <lvtk/ui.hpp>
#include <lvtk/ext/ui/idle.hpp>
#include <lvtk/ext/ui/parent.hpp>
#include <lvtk/ext/ui/resize.hpp>
#include <lvtk/options.hpp>

#include <lvtk/ui/nanovg.hpp>
#include <lvtk/ui/widget.hpp>
#include <lvtk/ui/main.hpp>

#include <lvtk/ext/urid.hpp>
#include <lvtk/weak_ref.hpp>

#define LVTK_VOLUME_UI_URI "http://lvtk.org/plugins/volume/ui"

using namespace lvtk;

class VolumeUI final : public UI<VolumeUI, Parent, Idle, URID, Options>
{
public:
    VolumeUI (const UIArgs& args)
        : UI (args)
    {
        for (const auto& opt : OptionArray (options()))
        {
            if (opt.key == map (LV2_UI__scaleFactor))
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
                     uint32_t format, const void*  buffer)
    {
#if 0
        if (format != 0 || size != sizeof (float))
            return;
        
        const float value = *((float*) buffer);
        const bool boolValue = value > 0.f;

        if (port >= RoboverbPorts::Comb_1 && port <= RoboverbPorts::Comb_8)
        {
           
        }
        else if (port >= RoboverbPorts::AllPass_1 && port <= RoboverbPorts::AllPass_4)
        {
           
        }
        else
        {
            switch (port)
            {
                case RoboverbPorts::Wet:
                    break;
                case RoboverbPorts::Dry:
                    break;
                case RoboverbPorts::RoomSize:
                    break;
                case RoboverbPorts::Damping:
                    break;
                case RoboverbPorts::Width:
                    break;
            }
        }
#endif
    }

    LV2UI_Widget get_widget() {
        if (content == nullptr) {
            content = std::make_unique<Content>();
            content->set_size (640, 360);
            content->set_visible (true);
            _main.elevate (*content, (uintptr_t)parent.get());
        }

        return *content;
    }

private:
    lvtk::Main _main { lvtk::Mode::MODULE, std::make_unique<lvtk::NanoVG>() };

    class Button : public Widget {
    public:
        Button() = default;
        virtual ~Button() = default;

        void paint (Graphics& g) override {
            g.set_color (color);
            g.fill_rect (bounds().at (0, 0).as<float>());
        }
        
        bool obstructed (Point<float> pos) override {
            return Widget::obstructed (pos);
        }

        void motion (InputEvent ev) override {
            // std::clog << __name << " motion: "
            //           << ev.pos.str() << std::endl;
        }

        void pressed (InputEvent ev) override {
            std::clog << __name << "   down: "
                      << ev.pos.str() << " bounds: "
                      << bounds().str() << std::endl;
        }

        void released (InputEvent ev) override {
            std::clog << __name << "     up: "
                      << ev.pos.str() << std::endl;
        }

        Color color { 0xff0000ff };
    };

    class Container : public Widget {
    public:
        Container() {
            add (button1);
            button1.set_visible (true);
            button1.__name = "button1";
            add (button2);
            button2.__name = "button2";
            button2.set_visible (true);
        }

        void resized() override {
            auto r1 = bounds().at (0, 0);
            std::clog << "container resized: " << r1.str() << std::endl;
            r1.width /= 2;
            auto r2 = r1;
            r2.x = r1.width;
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
            __name = "Content";
            add (buttons);
            buttons.set_visible (true);
            set_size (360, 240);
            set_visible (true);
        }

        ~Content() {

        }

        void motion (InputEvent ev) override {
            // std::clog << "content motion\n";
        }

        void resized() override {
            auto r = bounds().at (0, 0);
            r.x = 20;
            r.y = 20;
            r.width  -= (r.x * 2);
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

static UIDescriptor<VolumeUI> s_volume_ui (
    LVTK_VOLUME_UI_URI, { LV2_UI__parent }
);
