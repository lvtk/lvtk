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
    }

    bool obstructed (int x, int y) override { return true; }

    void motion (InputEvent ev) override {
    }

    void pressed (InputEvent ev) override {
    }

    void released (InputEvent ev) override {
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

} // namespace demo
} // namespace lvtk
