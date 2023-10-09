// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/widget.hpp>

#include "buttons.hpp"
#include "embedding.hpp"
#include "four_squares.hpp"
#include "menu_bar.hpp"
#include "utils.hpp"

namespace lvtk {
namespace demo {

#define SIDEBAR_WIDTH 200

enum ID {
    FOUR_SQUARES = 0,
    BUTTONS,
    ENTRY,
    SLIDERS,
    EMBED,
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
        case ENTRY:
            return "Entry";
            break;
        case SLIDERS:
            return "Sliders";
            break;
        case EMBED:
            return "Embed";
        default:
            break;
    }

    String demo ("Unknown Demo");
    demo << (did + 1);
    return demo;
}

class Content : public Widget {
public:
    Content();
    ~Content();

protected:
    /** @private */
    void resized() override;
    /** @private */
    void paint (Graphics& g) override;

private:
    int current_demo = -1;
    MenuBar menu;

    void run_demo (int index);

    class SideBar : public Widget {
    public:
        SideBar (Content& owner) : content (owner) {
            for (int i = 0; i < NUM_DEMOS; ++i) {
                auto box = add (new TextButton());
                box->set_visible (true);
                box->set_text (demo::name (i));
                box->on_clicked = [this, i]() { run_demo (i); };
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
            g.set_color (Color (0xff454545).darker (0.04f));
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
