
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

} // namespace demo
} // namespace lvtk
