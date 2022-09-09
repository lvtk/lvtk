#pragma once

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/widget.hpp>
#include <memory>

namespace lvtk {
namespace demo {

class Box : public lvtk::Widget {
public:
    Box() = default;
    virtual ~Box() = default;

    void paint (Graphics& g) override {
        g.set_color (is_on ? color_on : color_off);
        g.fill_rect (bounds().at (0, 0));
    }

    bool obstructed (int x, int y) override { return true; }

    void motion (InputEvent ev) override {
        // std::clog << __name << " motion: "
        //           << ev.pos.str() << std::endl;
    }

    void pressed (InputEvent ev) override {
        std::clog << __name << " pressed: "
                  << ev.pos.str() << "  bounds: "
                  << bounds().str() << std::endl;
    }

    void released (InputEvent ev) override {
        std::clog << __name << " released: "
                  << ev.pos.str() << "   bounds: "
                  << bounds().str() << std::endl;
        is_on = ! is_on;
        repaint();
    }

    bool is_on = false;
    Color color_on { 0x550000ff };
    Color color_off { 0x444444ff };
};

class Container : public Widget {
public:
    Container() {
        add (box1);
        box1.set_visible (true);
        box1.__name = "box1";
        add (box2);
        box2.__name = "box2";
        box2.set_visible (true);
    }

    void resized() override {
        auto r1 = bounds().at (0, 0);
        // std::clog << "container resized: " << r1.str() << std::endl;
        r1.width /= 2;
        auto r2 = r1;
        r2.x = r1.width;
        const int padding = 12;
        r1.x += padding;
        r1.y += padding;
        r1.width -= (padding * 2);
        r1.height -= (padding * 2);
        r2.x += padding;
        r2.y += padding;
        r2.width -= (padding * 2);
        r2.height -= (padding * 2);

        box1.set_bounds (r1);
        box2.set_bounds (r2);
    }

    void paint (Graphics& g) override {
        g.set_color (0x777777FF);
        g.fill_rect (bounds().at (0, 0));
    }

    Box box1, box2;
};

class Content : public Widget {
public:
    Content() {
        __name = "Content";
        add (buttons);
        buttons.set_visible (true);
        set_size (360, 240);
        set_visible (true);
    }

    ~Content() {
    }

    void motion (InputEvent ev) override {
        // std::clog << "content motion\n";
    }

    void resized() override {
        auto r = bounds().at (0, 0);
        r.x = 20;
        r.y = 20;
        r.width -= (r.x * 2);
        r.height -= (r.y * 2);
        buttons.set_bounds (r);
    }

    void paint (Graphics& g) override {
        g.set_color (Color (0x545454ff));
        g.fill_rect (bounds().at (0, 0).as<float>());
    }

private:
    Container buttons;
};

template <class Wgt>
static int run (lvtk::Main& context) {
    auto content = std::make_unique<Wgt>();
    content->set_size (640, 360);
    content->set_visible (true);

    try {
        context.elevate (*content, 0);
        bool quit = false;
        while (! quit) {
            context.loop (-1.0);
            quit = context.__quit_flag;
        }
    } catch (...) {
        std::clog << "fatal error in main loop\n";
        // std::clog << e.what() << std::endl;
        return 1;
    }

    content.reset();
    return 0;
}

static int run (lvtk::Main& context, int, char**) {
    return run<Content> (context);
}

} // namespace demo
} // namespace lvtk
