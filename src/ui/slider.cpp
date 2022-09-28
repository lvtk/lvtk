
#include <iostream>
#include <lvtk/ui/slider.hpp>

namespace lvtk {

namespace detail {

class Slider {
public:
    Slider (lvtk::Slider& o) : owner (o) {

    }

    void paint (Graphics& g) {
        auto r = owner.bounds().at(0);
        owner.style().draw_slider (g, owner, r, divider);
    }

    void resized() {
        
    }

    void pressed (const Event& ev) {
        divider = std::min ((float)owner.height(), std::max (0.f, ev.pos.y));
    }
    
    void drag (const Event& ev) {
        divider = std::min ((float)owner.height(), std::max (0.f, ev.pos.y));
        owner.repaint();
    }

private:
    friend class lvtk::Slider;
    lvtk::Slider& owner;
    float divider = 0.f;
    Range<double> range;
    double value;
};

}

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
void Slider::drag (const Event& ev) { impl->drag (ev); }

}
