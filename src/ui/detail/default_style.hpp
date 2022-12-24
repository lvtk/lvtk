
#pragma once

#include <lvtk/ui/graphics.hpp>
#include <lvtk/ui/slider.hpp>
#include <lvtk/ui/style.hpp>

namespace lvtk {
namespace detail {

class DefaultStyle : public Style {
public:
    DefaultStyle() {
        set_color (ColorID::BUTTON_BASE, Color (0x464646ff));
        set_color (ColorID::BUTTON_ON, Color (0x252525ff));
        set_color (ColorID::BUTTON_TEXT_OFF, Color (0xeeeeeeff));
        set_color (ColorID::BUTTON_TEXT_ON, Color (0xddddddff));

        set_color (ColorID::SLIDER_BASE, Color (0x141414ff));
        set_color (ColorID::SLIDER_THUMB, Color (0x451414ff));
    }

    ~DefaultStyle() {}

    void draw_button_shape (Graphics& g, lvtk::Button& w, bool highlight, bool down) override {
        auto bc = w.toggled() ? find_color (ColorID::BUTTON_ON) : find_color (ColorID::BUTTON_BASE);
        if (highlight || down) {
            if (! down)
                bc = bc.brighter (-0.015f);
            else
                bc = bc.brighter (-0.035f);
        }

        auto r = w.bounds().at (0);
        g.set_color (bc.brighter (-0.02f));
        auto cs = 2.2f;
        g.draw_rounded_rect (r, cs);
        g.set_color (bc);
        g.fill_rounded_rect (w.bounds().at (0), cs);
    }

    void draw_button_text (Graphics& g, lvtk::TextButton& w, bool highlight, bool down) override {
        auto c = find_color (w.toggled() ? ColorID::BUTTON_TEXT_ON : ColorID::BUTTON_TEXT_OFF);
        if (highlight || down)
            c = c.brighter (0.05f);
        g.set_color (c);

        auto r = w.bounds().at (0).as<float>();
        g.draw_text (w.text(), r, Align::CENTERED);
    }

    void draw_slider (Graphics& g, lvtk::Slider& slider, Bounds bounds, float pos) override {
        auto r = bounds.as<float>();
        switch (slider.type()) {
            case lvtk::Slider::HORIZONTAL_BAR:
            case lvtk::Slider::VERTICAL_BAR: {
                g.set_color (find_color (ColorID::SLIDER_BASE));
                g.fill_rect (r);
                g.set_color (find_color (ColorID::SLIDER_THUMB));

                if (slider.vertical()) {
                    r.slice_top (pos);
                } else {
                    r = r.slice_left (pos);
                }

                g.fill_rect (r);
                break;
            }

            case lvtk::Slider::HORIZONTAL:
            case lvtk::Slider::VERTICAL: {
                draw_slider_background (g, slider, bounds, pos);
                draw_slider_thumb (g, slider, bounds, pos);
                break;
            }
        }
    }
    
    void draw_slider_background (Graphics& g, lvtk::Slider& slider, Bounds bounds, float pos) override {
    }

    void draw_slider_thumb (Graphics& g, lvtk::Slider& slider, Bounds bounds, float pos) override {
    }
};
}}
