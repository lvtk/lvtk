// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/slider.hpp>

#include "demo.hpp"
#include "utils.hpp"

namespace lvtk {
namespace demo {

class Dials : public DemoWidget {
public:
    enum { total_dials = 10 };
    Dials() {
        for (int i = 0; i < total_dials; ++i) {
            dials.push_back (new Dial());
            auto& dial = *add (dials.back());
            dial.set_name (std::string ("Dial ") + std::to_string (i + 1));
            dial.set_range (0.0, 100.0);
            dial.on_value_changed = [this, &s = dial]() { update_text (s); };
        }

        add (reset_button);
        reset_button.on_clicked = std::bind (&Dials::reset_values, this);
        update_button_text();
        update_text (*dials.front());

        reset_values();
        show_all();
        set_size (640, 360);
    }

    ~Dials() {
        demo::delete_widgets (dials);
    }

    void reset_values() {
        for (auto dial : dials) {
            auto& span = dial->range();
            dial->set_value (span.min + span.diff() * 0.64, Notify::NONE);
        }
    }

private:
    void update_button_text() {
        reset_button.set_text ("Reset");
    }

    void update_text (Dial& dial) {
        last_value = dial.value();
        value_str  = std::string ("VALUE: ") + std::to_string (dial.value());
        min_str    = std::string ("MIN: ") + std::to_string (dial.range().min);
        max_str    = std::string ("MAX: ") + std::to_string (dial.range().max);
        repaint();
    }

    void resized() override {
        auto r1       = bounds().at (0).reduced (5, 5);
        const auto tr = r1.slice_top (40);
        r1.slice_bottom (16);

        int dial_width = 100;
        int pad        = 2;
        int box_size   = pad * 2 + dial_width;

        Bounds r2;
        for (auto s : dials) {
            if (r2.empty() || r2.x + box_size >= r1.x + r1.width)
                r2 = r1.slice_top (box_size);

            auto sr = r2.slice_left (box_size).reduced (pad / 2);
            s->set_bounds (sr.x, sr.y, sr.width, sr.height);
        }

        const int nbw = 120;
        reset_button.set_bounds (
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

private:
    std::vector<Dial*> dials;
    TextButton reset_button {};
    double last_value = 0.0;
    std::string value_str { "VALUE: N/A" };
    std::string min_str { "MIN: N/A" };
    std::string max_str { "MAX: N/A" };
};

std::unique_ptr<Widget> create_dials_demo() {
    return std::make_unique<Dials>();
}

} // namespace demo
} // namespace lvtk
