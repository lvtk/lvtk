
#pragma once

#include <lvtk/ui/widget.hpp>

namespace lvtk {
namespace demo {

class MenuBar : public Widget {
public:
    MenuBar()
        : file_menu (*this) {
        add (file);
        file.text = "File";
        file.set_visible (true);

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

}}
