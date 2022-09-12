#pragma once

#include <functional>
#include <lvtk/ui/button.hpp>

namespace lvtk {
namespace demo {

class Buttons : public Widget {
public:
    Buttons() {
        add (button1);
        button1.set_text ("Cancel");
        button1.set_visible (true);
        button1.clicked = [this]() {
            message = "Cancel clicked!";
            repaint();
            auto gp = button1.to_view_space (button1.bounds().pos());
            std::clog << "local: " << button1.bounds().pos().str() << " gp: " << gp.str()
                      << " pb: " << bounds().str() << std::endl;
        };

        add (button2);
        button2.set_text ("Apply");
        button2.set_visible (true);
        button2.clicked = [this]() { message = "Apply clicked!"; repaint(); };

        add (button3);
        button3.set_text ("Ok");
        button3.set_visible (true);
        button3.clicked = [this]() { message = "OK clicked!"; repaint(); };
    }

    ~Buttons() {}

protected:
    void paint (Graphics& g) override {
        g.set_color (0xeeeeeeff);
        g.text (message, width() / 2, height() / 2);
    }

    void resized() override {
        int pad = 4;
        auto r = bounds().at (0).slice_bottom (40).smaller (4);
        button1.set_bounds (r.slice_left (120));
        button3.set_bounds (r.slice_right (120));
        r.slice_right (pad);
        button2.set_bounds (r.slice_right (120));
    }

private:
    TextButton button1, button2, button3;
    String message;
};

} // namespace demo
} // namespace lvtk
