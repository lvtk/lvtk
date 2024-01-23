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
    enum { total_sliders = 11 };
    Sliders() {
        for (int i = 0; i < total_sliders; ++i) {
            sliders.push_back (new Slider());
            auto& slider = *add (sliders.back());
            slider.set_name (std::string ("Slider ") + std::to_string (i + 1));
            slider.set_type (slider_type);
            slider.set_range (0.0, 100.0);
            slider.set_value (slider.range().max * i / (total_sliders - 1.0), Notify::NONE);
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
        value_str  = std::string ("VALUE: ") + std::to_string (slider.value());
        min_str    = std::string ("MIN: ") + std::to_string (slider.range().min);
        max_str    = std::string ("MAX: ") + std::to_string (slider.range().max);
        repaint();
    }

    bool vertical() const noexcept {
        return slider_type == Slider::VERTICAL || slider_type == Slider::VERTICAL_BAR;
    }

    void resized() override {
        auto r1       = bounds().at (0).reduced (5, 5);
        const auto tr = r1.slice_top (40);
        r1.slice_bottom (16);

        int gap               = 10;
        auto hide_area        = vertical() ? bounds().at (0).slice_right (2)
                                           : bounds().at (0).slice_bottom (10);
        const int step_factor = vertical() ? r1.width : r1.height;
        int step              = std::min (37, std::max (28, step_factor / (int) sliders.size()));

        if (vertical()) {
            for (auto s : sliders) {
                s->set_bounds (r1.slice_left (step));
                s->set_visible (! hide_area.intersects (s->bounds()));
                r1.slice_left (gap);
            }
        } else {
            for (auto s : sliders) {
                s->set_bounds (r1.slice_top (step));
                s->set_visible (! hide_area.intersects (s->bounds()));
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
        auto label_width = [&] (std::string_view text) -> int {
            auto tm = g.context().text_metrics (text);
            return 10 + static_cast<int> (tm.width);
        };

        g.draw_text (min_str, r.as<float>(), Justify::BOTTOM_LEFT);
        r.slice_left (label_width (min_str));
        g.draw_text (max_str, r.as<float>(), Justify::BOTTOM_LEFT);
        r.slice_left (label_width (min_str));
        r.slice_right (8);
        g.draw_text (value_str, r.as<float>(), Justify::BOTTOM_RIGHT);
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
    std::string value_str { "VALUE: N/A" };
    std::string min_str { "MIN: N/A" };
    std::string max_str { "MAX: N/A" };
};

std::unique_ptr<Widget> create_sliders_demo() {
    return std::make_unique<Sliders>();
}

} // namespace demo
} // namespace lvtk
