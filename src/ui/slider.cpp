// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <lvtk/ui/path.hpp>
#include <lvtk/ui/slider.hpp>

namespace lvtk {

namespace detail {

template <typename T>
static inline T limit (const T& v, const T& a, const T& b) {
    return std::min (b, std::max (a, v));
}

// prototype dial draw function.  Eventually nees to move to style class.
inline static void draw_dial_proto (Graphics& g, lvtk::Dial& dial, Rectangle<double> r) {
    const auto radius    = std::min (r.width / 2, r.height / 2) - 2.0;
    const float center_x = r.x + r.width * 0.5;
    const float center_y = r.y + r.height * 0.5;
    const float rx       = center_x - radius;
    const float ry       = center_y - radius;
    const float rw       = radius * 2.0f;

    auto da0 = -2.356194; // -45 degrees.
    auto da1 = 2.356194;  //  45 degrees.

    Range<double> range (0.0, 1.0);
    const auto sliderPos = (float) range.convert (dial.range(), dial.value());
    const auto anchorPos = 0.0;

    const float angle  = da0 + sliderPos * (da1 - da0);
    const float anchor = da0 + anchorPos * (da1 - da0);
    const float a1     = angle < anchor ? angle : anchor;
    const float a2     = angle < anchor ? anchor : angle;
    lvtk::ignore (a1, a2);

    const bool is_mouse_over = false;

    if (radius > 12.0f) {
        int line_size = (int) std::max (2.0, std::min (20.0, radius * 0.085));
        lvtk::ignore (line_size);
        float line_trim       = radius > 32.f ? -3.0f : -2.f;
        float line_offset     = radius > 32.f ? -4.f : -1.f;
        const float thickness = 0.82f;
        const float csf       = rw - (rw * thickness);
        {
            const float csf = rw - (rw * thickness);
            Path filled;
            filled.add_ellipse (Rectangle<float> (rx, ry, rw, rw).reduced (csf));
            g.set_color (Color (0xffffffff).darker (1.f));
            g.fill_path (filled);
            g.set_color (Color (0xff000000).brighter (0.17f));
            g.stroke_path (filled);
        }

        if (true) //(dial.enabled())
            g.set_color (Color (0xffcc0000).with_alpha (is_mouse_over ? 1.0f : 0.88f));
        else
            g.set_color (Color (0x80808080));

        {
            g.context().save();
            g.context().transform (Transform::rotation (angle).translated (center_x, center_y));
            g.context().move_to (0.f, line_offset);
            g.context().line_to (0.f, -radius + csf + std::abs (line_trim));
            g.context().set_fill (Color (0xff000000));
            g.context().stroke();
            g.context().restore();
        }
    } else {
    }
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
        if (owner.vertical()) {
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

bool Slider::vertical() const noexcept {
    return impl->type == Slider::VERTICAL || impl->type == Slider::VERTICAL_BAR;
}

void Slider::set_type (Type type) { impl->set_type (type); }
Slider::Type Slider::type() const noexcept { return impl->type; }
void Slider::paint (Graphics& g) { impl->paint (g); }
void Slider::resized() { impl->resized(); }
void Slider::pressed (const Event& ev) { impl->pressed (ev); }
void Slider::drag (const Event& ev) { impl->drag (ev); }

class Dial::Impl {
public:
    Impl()  = default;
    ~Impl() = default;
    Point<float> down_pos {};
    double down_value  = 0.0;
    double sensitivity = 50;
};

Dial::Dial() : impl (new Impl()) {}
Dial::~Dial() {
    impl.reset();
}

void Dial::paint (Graphics& g) {
    detail::draw_dial_proto (g, *this, bounds().at (0).as<double>());
}

void Dial::resized() {}

void Dial::pressed (const Event& ev) {
    impl->down_value = value();
    impl->down_pos   = ev.pos;
}

void Dial::drag (const Event& ev) {
    // This works but is hacky and needs to be better.

    auto& down_pos   = impl->down_pos;
    auto& down_value = impl->down_value;

    bool horizontal = true;
    auto delta      = horizontal ? ev.pos.x - down_pos.x
                                 : ev.pos.y - down_pos.y;
    double new_pos  = 0;
    new_pos         = delta / double (horizontal ? width() : height());
    new_pos *= std::max (1.0, impl->sensitivity);
    new_pos += down_value;
    new_pos = detail::limit (new_pos, range().min, range().max);

    if (value() != new_pos) {
        set_value (new_pos, Notify::SYNC);
    }
}

} // namespace lvtk
