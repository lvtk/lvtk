// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "demo.hpp"

namespace lvtk {
namespace demo {

extern std::unique_ptr<Widget> create_entry_demo();
extern std::unique_ptr<Widget> create_sliders_demo();

Content::Content()
    : sidebar (*this) {
    set_name ("Demo Content");

    add (sidebar);
    sidebar.set_visible (true);
    set_size (540 + SIDEBAR_WIDTH, 360);

    add (menu);
    menu.set_visible (true);

    set_visible (true);
    sidebar.run_demo (0);
}

Content::~Content() {
}

void Content::resized() {
    auto r = bounds().at (0, 0);
    menu.set_bounds (r.slice_top (menu.height()));
    r.slice_top (1);
    sidebar.set_bounds (r.slice_left (SIDEBAR_WIDTH));
    if (demo) {
        demo->set_bounds (r.smaller (4));
    }
}

void Content::paint (Graphics& g) {
    g.set_color (Color (0x545454ff));
    g.fill_rect (bounds().at (0, 0).as<float>());
}

void Content::run_demo (int index) {
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
        case ENTRY:
            demo = create_entry_demo();
            break;
        case SLIDERS:
            demo = create_sliders_demo();
            break;
        case EMBED:
            demo.reset (new Embedding());
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
    repaint();
}

} // namespace demo
} // namespace lvtk
