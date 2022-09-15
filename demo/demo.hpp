#pragma once

#include <memory>

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/widget.hpp>

#include "buttons.hpp"
#include "four_squares.hpp"
#include "utils.hpp"

namespace lvtk {
namespace demo {

class MenuBar : public Widget {
public:
    MenuBar()
        : file_menu (*this) {
        add (file);
        file.text = "File";
        file.set_visible (true);
        file.clicked = [this]() {
            // file_menu.set_bounds (x() + 2, height() + 2, std::max(100, file.width() - 4), 300);
            // file_menu.set_visible (true);
            // if (auto view = find_view())
            //     view->elevate (file_menu);
        };

        add (help);
        help.set_visible (true);
        help.text = "Help";
        set_size (300, 24);
    }
    ~MenuBar() {}

protected:
    bool obstructed (int, int) override { return true; }

    void paint (Graphics& g) override {
        g.set_color (Color (0xeeeeeeff));
        g.fill_rect (bounds().at (0));
    }

    void resized() override {
        auto b = bounds().smaller (0, 1);
        file.set_bounds (b.slice_left (48));
        b.slice_left (2);
        help.set_bounds (b.slice_left (48));
    }

private:
    class Item : public Widget {
    public:
        std::function<void()> clicked;

        void paint (Graphics& g) override {
            g.set_color (Color (0x111111ff));
            g.draw_text (text, bounds().at (0).as<float>(), Alignment::CENTERED);
        }

        void pressed (InputEvent ev) override {
            if (clicked)
                clicked();
        }

        bool obstructed (int, int) override { return true; }
        String text;
    };

    class Menu : public Widget {
    public:
        Menu (MenuBar& bar) : menubar (bar) {}
        ~Menu() {}
        bool obstructed (int, int) override { return true; }
        void paint (Graphics& g) override {
            g.set_color (Color (0xefefefff));
            g.fill_rect (bounds().at (0));
        }

        void pressed (InputEvent ev) override {
            set_visible (false);
            if (auto root = menubar.find_view())
                root->repaint ({});
        };
        MenuBar& menubar;
    } file_menu;

    Item file, help;
};

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
        default:
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
        menu.set_bounds (r.slice_top (menu.height()));
        r.slice_top (1);
        sidebar.set_bounds (r.slice_left (SIDEBAR_WIDTH));
        if (demo) {
            demo->set_bounds (r.smaller (4));
        }
    }

    void paint (Graphics& g) override {
        g.set_color (Color (0x545454ff));
        g.fill_rect (bounds().at (0, 0).as<float>());
    }

private:
    Main& main;
    int current_demo = -1;
    MenuBar menu;

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
            default:
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
                box->clicked = [this, i]() { run_demo (i); };
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

} // namespace demo
} // namespace lvtk
