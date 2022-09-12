
#pragma once

#include <functional>
#include <lvtk/ui/widget.hpp>

namespace lvtk {

class Button : public lvtk::Widget {
public:
    Button() = default;
    virtual ~Button() = default;

    std::function<void()> clicked;

    bool toggled() const noexcept { return _toggled; }
    void toggle (bool toggled) {
        if (toggled == _toggled)
            return;
        _toggled = toggled;
        repaint();
    }

protected:
    virtual void paint_button (Graphics& g, bool highlight, bool down) {}

    void paint (Graphics& g) override {
        paint_button (g, _over, _down);
    }

    bool obstructed (int x, int y) override { return true; }

    void pressed (InputEvent ev) override {
        _down = true;
        repaint();
    }

    void released (InputEvent ev) override {
        _down = false;
        if (contains (ev.pos))
            notify_clicked();
        repaint();
    }

private:
    bool _toggled = false;
    bool _down = false, _over = false;

    void notify_clicked() {
        if (clicked)
            clicked();
    }
};

class TextButton : public Button {
public:
    TextButton() = default;
    virtual ~TextButton() = default;

    String text() const noexcept { return _text; }

    void set_text (const String& text) {
        if (text == _text)
            return;
        _text = text;
        repaint();
    }

protected:
    void paint_button (Graphics& g, bool highlight, bool down) override {
        auto& s = style();
        s.draw_button_shape (g, *this, highlight, down);
        s.draw_button_text (g, *this, highlight, down);
    }

private:
    String _text;
};

} // namespace lvtk
