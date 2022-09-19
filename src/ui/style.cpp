
#include <lvtk/ui/style.hpp>

namespace lvtk {

Style::Style() {
    _weak_status.reset (this);
}

Style::~Style() {
    _weak_status.reset();
}

void Style::set_color (int ID, Color color) {
    _colors.insert (ColorItem { ID, color });
}

Color Style::find_color (int ID) const noexcept {
    if (_colors.empty())
        return {};
    auto it = _colors.find (ColorItem { ID, {} });
    if (it != _colors.end())
        return (*it).color;
    return {};
}

}
