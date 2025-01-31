// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "demo.hpp"
#include "buttons.hpp"
#include "embedding.hpp"

namespace lvtk {
namespace demo {

extern std::unique_ptr<Widget> create_entry_demo();
extern std::unique_ptr<Widget> create_sliders_demo();
extern std::unique_ptr<Widget> create_images_demo();
extern std::unique_ptr<Widget> create_dials_demo();

Content::Content()
    : sidebar (*this) {
    set_name ("Demo Content");
    set_opaque (true);

    add (sidebar);
    sidebar.set_visible (true);
    set_size (550 + SIDEBAR_WIDTH, 400);

    add (menu);
    menu.set_visible (false);

    set_visible (true);
    sidebar.run_demo (0);
}

Content::~Content() {
}

void Content::resized() {
    auto r = bounds().at (0, 0);

    if (menu.visible())
        menu.set_bounds (r.slice_top (menu.height()));

    r.slice_top (4);
    r.slice_left (4);
    sidebar.set_bounds (r.slice_left (SIDEBAR_WIDTH));
    if (demo) {
        demo->set_bounds (r.smaller (4));
    }
}

void Content::paint (Graphics& g) {
    g.set_color (Color (0xff252525));
    // g.set_color (Color (0xff550000));
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
        case BUTTONS:
            demo.reset (new Buttons());
            break;
        case IMAGES:
            demo = create_images_demo();
            break;
        case ENTRY:
            demo = create_entry_demo();
            break;
        case DIALS:
            demo = create_dials_demo();
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
        if (demo->name().empty())
            demo->set_name (demo::name (index));
        demo->set_visible (true);
    }

    current_demo = index;
    resized();
    repaint();
}

} // namespace demo
} // namespace lvtk
