// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <functional>
#include <iostream>

#include "demo.hpp"
#include <lvtk/ui/button.hpp>

namespace lvtk {
namespace demo {

class Buttons : public DemoWidget {
public:
    Buttons() {
        add (button1);
        button1.set_text ("Quit");
        button1.set_name ("Quit Button");
        button1.set_visible (true);
        button1.on_clicked = [this]() {
            if (auto v = find_view())
                v->main().quit();
            repaint();
        };

        add (button2);
        button2.set_name ("Apply Button");
        button2.set_text ("Apply");
        button2.set_visible (true);
        button2.on_clicked = [this]() { message = "Apply clicked!"; repaint(); };

        add (button3);
        button3.set_text ("Ok");
        button3.set_visible (true);
        button3.on_clicked = [this]() { message = "OK clicked!"; repaint(); };
    }

    ~Buttons() {}

protected:
    void paint (Graphics& g) override {
        DemoWidget::paint (g);

        g.set_color (0xffeeeeee);
        g.set_font (Font (24.f));

        auto r = bounds().at (0);
        r.slice_bottom (40);
        g.draw_text (message, r.as<float>(), Align::CENTERED);
    }

    void resized() override {
        int pad = 4;
        auto r  = bounds().at (0).slice_bottom (40).smaller (4);
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
