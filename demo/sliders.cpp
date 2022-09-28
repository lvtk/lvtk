
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
        add (slider2);
        slider2.set_name ("Vertical 2");
        slider2.set_range (-50.0, 50.0);

        show_all();
        set_size (640, 360);
    }

    ~Sliders() {}

private:
    void resized() override {
        auto r1 = bounds().at (0).smaller (20).slice_top (160);
        slider1.set_bounds (r1.slice_left (32));
        r1.slice_left(10);
        slider2.set_bounds (r1.slice_left (32));
    }

    void paint (Graphics& g) override {
        g.set_color (0x777777ff);
        g.fill_rect (bounds().at (0, 0));
    }

private:
    Slider slider1, slider2;
};

std::unique_ptr<Widget> create_sliders_demo() {
    return std::make_unique<Sliders>();
}

} // namespace demo
} // namespace lvtk
