// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/entry.hpp>
#include <lvtk/ui/slider.hpp>
#include <lvtk/ui/widget.hpp>

#include "demo.hpp"
#include "utils.hpp"

namespace lvtk {
namespace demo {

class Sliders : public DemoWidget {
public:
    Sliders() {
        for (int i = 0; i < 8; ++i) {
            sliders.push_back (new Slider());
            auto& slider = *add (sliders.back());
            slider.set_name (std::string ("Slider ") + std::to_string (i + 1));
            slider.set_type (slider_type);
            slider.set_range (0.0, 100.0);
            slider.set_value (slider.range().max * i / 7.0, Notify::NONE);
            slider.on_value_changed = [this, &s = slider]() { update_text (s); };
        }

        add (next_button);
        next_button.on_clicked = [this]() { next_type(); };
        update_button_text();
        update_text (*sliders.front());

        show_all();
        set_size (640, 360);
    }

    ~Sliders() {
        demo::delete_widgets (sliders);
    }

private:
    std::string slider_type_name() {
        std::string name;
        switch (slider_type) {
            case Slider::VERTICAL:
                name = "Vertical";
                break;
            case Slider::VERTICAL_BAR:
                name = "Vertical Bar";
                break;
            case Slider::HORIZONTAL:
                name = "Horizontal";
                break;
            case Slider::HORIZONTAL_BAR:
                name = "Horizontal Bar";
                break;
        }
        return name;
    }
    void update_button_text() {
        next_button.set_text (slider_type_name());
    }

    void update_text (Slider& slider) {
        last_value = slider.value();
        value_str  = std::string ("value: ") + std::to_string (slider.value());
        min_str    = std::string ("min: ") + std::to_string (slider.range().min);
        max_str    = std::string ("max: ") + std::to_string (slider.range().max);
        repaint();
    }

    void resized() override {
        auto r1 = bounds().at (0);
        r1.reduce (4, 0);
        const auto tr = r1.slice_top (40);
        const auto br = r1.slice_bottom (26);
        r1.slice_left (10);
        int gap = 4;

        if (slider_type == Slider::VERTICAL || slider_type == Slider::VERTICAL_BAR) {
            int step       = std::max (28, r1.width / (int) sliders.size());
            auto hide_area = bounds().at (0).slice_right (2);

            for (auto s : sliders) {
                s->set_bounds (r1.slice_left (step));
                s->set_visible (! hide_area.intersects (s->bounds()));
                r1.slice_left (gap);
            }
        } else {
            int step = std::max (28, r1.height / (int) sliders.size());
            for (auto s : sliders) {
                s->set_bounds (r1.slice_top (step));
                s->set_visible (! br.intersects (s->bounds()));
                r1.slice_top (gap);
            }
        }

        const int nbw = 120;
        next_button.set_bounds (
            width() / 2 - nbw / 2,
            tr.y + (tr.height / 2) - 15,
            nbw,
            30);
    }

    void paint (Graphics& g) override {
        DemoWidget::paint (g);

        g.set_font (11.0);
        g.set_color (Color (0xffeeeeee));
        auto r = bounds().at (0);
        r.reduce (4, 4);
        r = r.slice_bottom (30);

        r.slice_left (8);
        g.draw_text (min_str, r.as<float>(), Align::BOTTOM_LEFT);
        r.slice_left (120);
        g.draw_text (max_str, r.as<float>(), Align::BOTTOM_LEFT);
        r.slice_left (120);
        r.slice_right (8);
        g.draw_text (value_str, r.as<float>(), Align::BOTTOM_RIGHT);
    }

    void set_type (Slider::Type new_type) {
        if (new_type == slider_type)
            return;
        slider_type = new_type;
        for (auto s : sliders) {
            s->set_type (slider_type);
            s->set_visible (true);
        }
        update_button_text();
        resized();
    }

    void next_type() {
        if (slider_type == 3) {
            set_type (static_cast<Slider::Type> (0));
        } else {
            set_type (static_cast<Slider::Type> (1 + slider_type));
        }
    }

private:
    std::vector<Slider*> sliders;
    TextButton next_button {};
    Slider::Type slider_type { Slider::VERTICAL_BAR };
    double last_value = 0.0;
    std::string value_str { "value: N/A" };
    std::string min_str { "min: N/A" };
    std::string max_str { "max: N/A" };
};

std::unique_ptr<Widget> create_sliders_demo() {
    return std::make_unique<Sliders>();
}

} // namespace demo
} // namespace lvtk
