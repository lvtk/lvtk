#pragma once

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/widget.hpp>
#include <memory>

namespace lvtk {
namespace demo {

template <class Wgt>
static inline void delete_widgets (std::vector<Wgt*>& vec) {
    for (auto w : vec)
        delete w;
    vec.clear();
}

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
        for (int i = 0; i < 4; ++i) {
            auto box = add (new Box());
            box->set_visible (true);
            box->__name = std::string ("box") + std::to_string (i + 1);
            boxes.push_back (box);
        }

        set_size (640, 360);
    }

    ~Container() {
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

    std::vector<Box*> boxes;
};

class Content : public Widget {
public:
    Content() {
        __name = "Content";
        add (buttons);
        buttons.set_visible (true);
        set_size (400, 400);
        set_visible (true);
    }

    ~Content() {
    }

    void motion (InputEvent ev) override {
        // std::clog << "content motion\n";
    }

    void resized() override {
        auto r = bounds().at (0, 0);
        r.x = 4;
        r.y = 4;
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
    try {
        auto content = std::make_unique<Wgt>();
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

    return 0;
}

static int run (lvtk::Main& context, int, char**) {
    return run<Content> (context);
}

} // namespace demo
} // namespace lvtk
