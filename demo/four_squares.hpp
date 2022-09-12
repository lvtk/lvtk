
#pragma once

#include "utils.hpp"
#include <lvtk/ui/widget.hpp>

namespace lvtk {
namespace demo {

class Box : public lvtk::Widget {
public:
    Box() = default;
    virtual ~Box() = default;

    void paint (Graphics& g) override {
        g.set_color (is_on ? color_on : color_off);
        g.fill_rect (bounds().at (0, 0));
        g.set_color (color_text);
        g.text (__name, width() / 2, height() / 2);
    }

    bool obstructed (int x, int y) override { return true; }

    void motion (InputEvent ev) override {
        // std::clog << __name << " motion: "
        //           << ev.pos.str() << std::endl;
    }

    void pressed (InputEvent ev) override {
        is_on = ! is_on;
        repaint();
    }

    bool is_on = false;
    Color color_text { 0xffffffff };
    Color color_on { 0x550000ff };
    Color color_off { 0x444444ff };
};

class FourSquares : public Widget {
public:
    FourSquares() {
        for (int i = 0; i < 4; ++i) {
            auto box = add (new Box());
            box->set_visible (true);
            box->__name = std::string ("Box ") + std::to_string (i + 1);
            boxes.push_back (box);
        }

        set_size (640, 360);
    }

    ~FourSquares() {
        delete_widgets (boxes);
    }

    void resized() override {
        auto r1 = bounds().at (0);
        for (int i = 0; i < 4; ++i) {
            auto r2 = r1;
            r2.width = r1.width / 2;
            r2.height = r1.height / 2;
            auto box = boxes[i];

            switch (i) {
                case 0: // top left
                    r2.x = 0;
                    r2.y = 0;
                    break;
                case 1: // top right
                    r2.x = r2.width;
                    r2.y = 0;
                    break;
                case 2: // bottom left
                    r2.x = 0;
                    r2.y = r2.height;
                    break;
                case 3: // bottom right
                    r2.x = r2.width;
                    r2.y = r2.height;
                    break;
            }

            box->set_bounds (r2.smaller (4));
        }
    }

    void paint (Graphics& g) override {
        g.set_color (0x777777FF);
        g.fill_rect (bounds().at (0, 0));
    }

private:
    std::vector<Box*> boxes;
};

} // namespace demo
} // namespace lvtk
