
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
        slider1.__value_changed = [this]() { update_text (slider1); };
        slider1.set_value (90.0);

        add (slider2);
        slider2.set_name ("Vertical 2");
        slider2.set_range (-90.0, 24.0);
        slider2.__value_changed = [this]() { update_text (slider2); };
        slider2.set_value (0.0);

        show_all();
        set_size (640, 360);
    }

    ~Sliders() {}

private:
    void update_text (Slider& slider) {
        value_str = std::string ("value: ") + std::to_string (slider.value());
        min_str   = std::string ("min: ") + std::to_string (slider.min_value());
        max_str   = std::string ("max: ") + std::to_string (slider.max_value());
        repaint();
    }

    void resized() override {
        auto r1 = bounds().at (0).smaller (20).slice_top (160);
        slider1.set_bounds (r1.slice_left (32));
        r1.slice_left (10);
        slider2.set_bounds (r1.slice_left (32));
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
    }

private:
    Slider slider1, slider2;
    std::string value_str { "value: N/A" };
    std::string min_str { "min: N/A" };
    std::string max_str { "max: N/A" };
};

std::unique_ptr<Widget> create_sliders_demo() {
    return std::make_unique<Sliders>();
}

} // namespace demo
} // namespace lvtk
