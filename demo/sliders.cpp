// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/entry.hpp>
#include <lvtk/ui/slider.hpp>
#include <lvtk/ui/widget.hpp>

#include "utils.hpp"

namespace lvtk {
namespace demo {

class Sliders : public Widget {
public:
    Sliders() {
        add (slider1);
        slider1.set_name ("Vertical 1");
        slider1.set_range (0.0, 100.0);
        slider1.on_value_changed = [this]() { update_text (slider1); };
        slider1.set_value (90.0, Notify::SYNC);

        add (slider2);
        slider2.set_name ("Vertical 2");
        slider2.set_range (-90.0, 24.0);
        slider2.on_value_changed = [this]() { update_text (slider2); };
        slider2.set_value (0.0, Notify::SYNC);
        slider2.set_type (Slider::HORIZONTAL_BAR);

        show_all();
        set_size (640, 360);
    }

    ~Sliders() {}

private:
    void update_text (Slider& slider) {
        last_value = slider.value();
        value_str  = std::string ("value: ") + std::to_string (slider.value());
        min_str    = std::string ("min: ") + std::to_string (slider.range().min);
        max_str    = std::string ("max: ") + std::to_string (slider.range().max);
        repaint();
    }

    void resized() override {
        auto r1 = bounds().at (0).smaller (20).slice_top (height() - 64);
        slider1.set_bounds (r1.slice_left (32));
        r1.slice_left (10);
        r1 = r1.slice_bottom (32);
        r1.slice_right (10);
        slider2.set_bounds (r1);

        value_box.x      = slider1.x() + slider1.width() + 10;
        value_box.y      = slider1.y();
        value_box.width  = width() - value_box.x;
        value_box.height = height() - value_box.y - 64;
    }

    void paint (Graphics& g) override {
        g.set_color (0x777777ff);
        g.fill_rect (bounds().at (0, 0));

        g.set_color (Color (0xeeeeeeff));
        auto r = bounds().at (0).slice_bottom (30);
        r.slice_left (8);
        g.draw_text (min_str, r.as<float>(), Align::LEFT_MIDDLE);
        r.slice_left (120);
        g.draw_text (max_str, r.as<float>(), Align::LEFT_MIDDLE);
        r.slice_left (120);
        r.slice_right (8);
        g.draw_text (value_str, r.as<float>(), Align::RIGHT_MIDDLE);
        g.set_font (Font (64.f));
        g.draw_text (std::to_string (last_value),
                     value_box.as<float>(),
                     Align::CENTERED);
    }

private:
    Slider slider1, slider2;
    double last_value = 0.0;
    std::string value_str { "value: N/A" };
    std::string min_str { "min: N/A" };
    std::string max_str { "max: N/A" };
    Bounds value_box;
};

std::unique_ptr<Widget> create_sliders_demo() {
    return std::make_unique<Sliders>();
}

} // namespace demo
} // namespace lvtk
