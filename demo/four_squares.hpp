
#pragma once

#include "utils.hpp"
#include <lvtk/ui/widget.hpp>

namespace lvtk {
namespace demo {

class Box : public lvtk::Widget {
public:
    Box()          = default;
    virtual ~Box() = default;

    void paint (Graphics& g) override {
        auto fr = bounds().at (0, 0).as<float>();
        g.set_color (color_off);
        g.fill_rect (fr);

        g.set_color (color_on);
        fr.slice_top (divider_y);
        g.fill_rect (fr);
        g.set_color (color_text);

        auto r = bounds().at (0).as<float>().slice_bottom (height() / 2);

        g.set_font (Font (12.f));
        g.draw_text (name(), r, Alignment::CENTERED);
    }

    bool obstructed (int x, int y) override { return true; }

    void drag (const Event& ev) override {
        update_divider_y (ev);
        repaint();
    }

    void pressed (const Event& ev) override {
        update_divider_y (ev);
        is_on = true;
        repaint();
    }

    void released (const Event& ev) override {
        update_divider_y (ev);
        is_on = false;
        repaint();
    }

    void update_divider_y (const Event& ev) {
        divider_y = std::min ((float) height(), std::max (0.f, ev.pos.y));
    }

    bool is_on = false;
    Color color_text { 0xffffffff };
    Color color_on { 0x550000ff };
    Color color_off { 0x141414ff };
    float divider_y = 0.f;
};

class FourSquares : public Widget {
public:
    FourSquares() {
        for (int i = 0; i < 4; ++i) {
            auto box = add (new Box());
            box->set_visible (true);
            box->set_name (std::string ("Box ") + std::to_string (i + 1));
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
            auto r2   = r1;
            r2.width  = r1.width / 2;
            r2.height = r1.height / 2;
            auto box  = boxes[i];

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
