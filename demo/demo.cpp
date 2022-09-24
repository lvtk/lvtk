
#include "demo.hpp"

namespace lvtk {
namespace demo {

Content::Content()
    : sidebar (*this) {
    __name = "Content";

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

void Content::motion (InputEvent ev) {}

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
        case EMBEDDING:
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
}

} // namespace demo
} // namespace lvtk
