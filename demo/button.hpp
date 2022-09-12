#pragma once

#include <functional>
#include <lvtk/ui/widget.hpp>

namespace lvtk {
namespace demo {

class Button : public lvtk::Widget {
public:
    Button() = default;
    virtual ~Button() = default;

    std::function<void()> clicked;

    void paint (Graphics& g) override {
        g.set_color (toggled() ? color_on : color_off);
        g.fill_rect (bounds().at (0));
        g.set_color (0xffffffff);
        g.text (__name, 10, 12);
    }

    bool obstructed (int x, int y) override { return true; }

    void pressed (InputEvent ev) override {}

    void released (InputEvent ev) override {
        if (contains (ev.pos))
            notify_clicked();
    }

    bool toggled() const noexcept { return _toggled; }
    void toggle (bool toggled) {
        if (toggled == _toggled)
            return;
        _toggled = toggled;
        repaint();
    }

private:
    bool _toggled = false;
    Color color_on { 0x550000ff };
    Color color_off { 0x444444ff };
    void notify_clicked() {
        if (clicked)
            clicked();
    }
};

class Buttons : public Widget {
public:
    Buttons() {
        add (button1);
        button1.__name = "Text Button 1";
        button1.set_visible (true);
        add (button2);
        button2.__name = "Text Button 2";
        button2.set_visible (true);
        add (button3);
        button3.__name = "Text Button 3";
        button3.set_visible (true);
    }

    ~Buttons() {}

    void resized() override {
        int pad = 2;
        auto r = bounds().at(0).slice_bottom (40).smaller (4);
        button1.set_bounds (r.slice_left (120));
        r.slice_left (pad);
        button2.set_bounds (r.slice_left (120));
        button3.set_bounds (r.slice_right (120));
    }

private:
    Button button1, button2, button3;
};

} // namespace demo
} // namespace lvtk
