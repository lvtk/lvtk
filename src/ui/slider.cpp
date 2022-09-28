
#include <iostream>
#include <lvtk/ui/slider.hpp>

namespace lvtk {

namespace detail {

template <typename T>
T limit (const T& v, const T& a, const T& b) {
    return std::min (b, std::max (a, v));
}

class Slider {
public:
    Slider (lvtk::Slider& o) : owner (o) {
    }

    void paint (Graphics& g) {
        auto r = owner.bounds().at (0);
        owner.style().draw_slider (g, owner, r, divider);
    }

    void resized() {
    }

    void pressed (const Event& ev) {
        drag (ev);
    }

    void drag (const Event& ev) {
        divider          = detail::limit (ev.pos.y, 0.f, (float) owner.height());
        double min_coord = 0.0;
        double max_coord = (double) owner.height();

        const double ratio     = 1.0 - (divider - min_coord) / (max_coord - min_coord);
        const double new_value = (ratio * (owner.max_value() - owner.min_value())) + owner.min_value();
        if (owner.value() != new_value) {
            owner.set_value (new_value);
            if (owner.__value_changed)
                owner.__value_changed();
            // std::clog << "[slider] ratio: " << ratio
            //           << " new_value: " << owner.value() << std::endl;
        }

        owner.repaint();
    }

private:
    friend class lvtk::Slider;
    lvtk::Slider& owner;
    float divider = 0.f;
    Range<double> range;
    double value;
};

} // namespace detail

Ranged::Ranged() {}
Ranged::~Ranged() {}

Slider::Slider() {
    impl = std::make_unique<detail::Slider> (*this);
}

Slider::~Slider() {
    impl.reset();
}

void Slider::paint (Graphics& g) {
    impl->paint (g);
}

void Slider::resized() { impl->resized(); }
void Slider::pressed (const Event& ev) { impl->pressed (ev); }
void Slider::drag (const Event& ev) { impl->drag (ev); }

} // namespace lvtk
