// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>
#include <lvtk/ui/slider.hpp>

namespace lvtk {

namespace detail {

template <typename T>
static inline T limit (const T& v, const T& a, const T& b) {
    return std::min (b, std::max (a, v));
}

class Ranged {
public:
    Ranged (lvtk::Ranged& o) : owner (o) {}

private:
    friend class lvtk::Ranged;
    lvtk::Ranged& owner;
    Range<double> range;
    double value = 0.0;
};

class Slider {
public:
    using Type = lvtk::Slider::Type;

    Slider (lvtk::Slider& o) : owner (o) {
    }

    void paint (Graphics& g) {
        auto r = owner.bounds().at (0);
        owner.style().draw_slider (g, owner, r, divider);
    }

    void resized() {
        Range<double> pixel_range;
        pixel_range.min          = 0.0;
        const auto current_value = owner.value();
        float new_divider        = divider;
        if (type == Type::VERTICAL || type == Type::VERTICAL_BAR) {
            pixel_range.max = (double) owner.height();
            new_divider     = pixel_range.convert (owner.range(), current_value);
            new_divider     = pixel_range.max - new_divider;
        } else {
            pixel_range.max = (double) owner.width();
            new_divider     = pixel_range.convert (owner.range(), current_value);
        }

        if (new_divider != divider) {
            divider = new_divider;
            owner.repaint();
        }
    }

    void pressed (const Event& ev) {
        drag (ev);
    }

    void drag (const Event& ev) {
        Range<double> pixel_range;
        pixel_range.min  = 0.0;
        double new_value = owner.value();

        if (type == Type::VERTICAL || type == Type::VERTICAL_BAR) {
            divider         = detail::limit (ev.pos.y, 0.f, (float) owner.height());
            pixel_range.max = (double) owner.height();
            new_value       = owner.range().convert (pixel_range, divider);
            new_value       = owner.range().max - new_value;
        } else {
            divider         = detail::limit (ev.pos.x, 0.f, (float) owner.width());
            pixel_range.max = (double) owner.width();
            new_value       = owner.range().convert (pixel_range, divider);
        }

        owner.set_value (new_value, Notify::SYNC);
    }

    void set_type (Type t) {
        if (type == t)
            return;
        type = t;
        owner.resized();
        owner.repaint();
    }

private:
    friend class lvtk::Slider;
    lvtk::Slider& owner;
    float divider = 0.f;
    Type type { Type::VERTICAL_BAR };
};

} // namespace detail

Ranged::Ranged() : impl (std::make_unique<detail::Ranged> (*this)) {}
Ranged::~Ranged() { impl.reset(); }

const Range<double>& Ranged::range() const noexcept { return impl->range; }
double Ranged::value() const noexcept { return impl->value; }

void Ranged::set_value (double value, Notify notify) {
    if (impl->value == value)
        return;

    impl->value = value;

    resized();
    repaint();

    if (notify == Notify::NONE)
        return;

    if (on_value_changed) {
        if (notify == Notify::SYNC)
            on_value_changed();
        else { /* trigger async callback somehow */
        }
    }
}

void Ranged::set_range (double min, double max) {
    if (min >= max)
        return;
    if (min != impl->range.min || max != impl->range.max) {
        impl->range.min = min;
        impl->range.max = max;
    }
}

Slider::Slider() {
    impl = std::make_unique<detail::Slider> (*this);
}

Slider::~Slider() { impl.reset(); }
void Slider::set_type (Type type) { impl->set_type (type); }
Slider::Type Slider::type() const noexcept { return impl->type; }
bool Slider::vertical() const noexcept { return impl->type == Slider::VERTICAL || impl->type == Slider::VERTICAL_BAR; }
void Slider::paint (Graphics& g) { impl->paint (g); }
void Slider::resized() { impl->resized(); }
void Slider::pressed (const Event& ev) { impl->pressed (ev); }
void Slider::drag (const Event& ev) { impl->drag (ev); }

} // namespace lvtk
