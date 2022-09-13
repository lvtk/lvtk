#pragma once

#include <memory>

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/widget.hpp>

#include "buttons.hpp"
#include "four_squares.hpp"
#include "utils.hpp"

namespace lvtk {
namespace demo {

#define SIDEBAR_WIDTH 200

enum ID {
    FOUR_SQUARES = 0,
    BUTTONS,
    NUM_DEMOS
};

static std::string name (int did) {
    switch (did) {
        case FOUR_SQUARES:
            return "Four Squares";
            break;
        case BUTTONS:
            return "Buttons";
            break;
    }

    String demo ("Demo ");
    demo << (did + 1);
    return demo;
}

class Content : public Widget {
public:
    Content (Main& m);
    ~Content();

    void motion (InputEvent ev) override {}

    void resized() override {
        auto r = bounds().at (0, 0);
        r.width = SIDEBAR_WIDTH;
        sidebar.set_bounds (r);
        if (demo) {
            auto r2 = bounds().at (0);
            r2.x = r.width;
            r2.width -= r.width;
            demo->set_bounds (r2.smaller (4));
        }
    }

    void paint (Graphics& g) override {
        g.set_color (Color (0x545454ff));
        g.fill_rect (bounds().at (0, 0).as<float>());
    }

private:
    Main& main;
    int current_demo = -1;

    void run_demo (int index) {
        if (current_demo == index)
            return;

        if (demo != nullptr) {
            remove (*demo);
            demo.reset();
        }

        switch (index) {
            case FOUR_SQUARES:
                demo.reset (new FourSquares());
                break;
            case BUTTONS:
                demo.reset (new Buttons());
                break;
        }

        if (demo) {
            add (*demo);
            demo->set_visible (true);
        }

        current_demo = index;
        resized();
    }

    class SideBar : public Widget {
    public:
        SideBar (Content& owner) : content (owner) {
            for (int i = 0; i < NUM_DEMOS; ++i) {
                auto box = add (new TextButton());
                box->set_visible (true);
                box->set_text (demo::name (i));
                box->clicked = std::bind (&SideBar::run_demo, this, i);
                demos.push_back (box);
            }

            set_size (640, 360);
        }

        ~SideBar() {
            delete_widgets (demos);
        }

        void resized() override {
            auto r1 = bounds().at (0);
            for (auto box : demos) {
                auto r2 = r1.slice_top (40).smaller (2, 1);
                box->set_bounds (r2);
            }
        }

        void paint (Graphics& g) override {
            g.set_color (0x777777FF);
            g.fill_rect (bounds().at (0, 0));
        }

    private:
        friend class Content;
        Content& content;
        std::vector<Button*> demos;

        void run_demo (int index) {
            for (int i = 0; i < (int) demos.size(); ++i) {
                demos[i]->toggle (i == index);
            }

            content.run_demo (index);
        }
    } sidebar;

    std::unique_ptr<Widget> demo;
};

template <class Wgt>
static int run (lvtk::Main& context) {
    try {
        auto content = std::make_unique<Wgt> (context);
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
