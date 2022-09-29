// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <functional>

#include <lvtk/lvtk.h>
#include <lvtk/ui/widget.hpp>

namespace lvtk {

/** A range of two values
    E.g. min and max with some conversion helpers
 */
template <typename Val>
class Range {
public:
    Range() = default;
    Range (Val minimum, Val maximum) : min (minimum), max (maximum) {}

    /** Returns true if min == max */
    constexpr bool empty() const noexcept { return min == max; }

    /** Returns the difference (max - min) */
    constexpr Val diff() const noexcept { return max - min; }

    /** Returns the proportion of val within min/max
        i.e. convert val to 0.0 - 1.0 
     */
    constexpr double ratio (Val val) const noexcept {
        return double (val - min) / double (max - min);
    }

    /** Convert a value from another range to fall within this one */
    constexpr Val convert (const Range<Val>& o, const Val& v) const noexcept {
        return (o.ratio (v) * static_cast<double> (diff())) + min;
    }

    bool operator== (const Range& o) const noexcept { return min == o.min && max == o.max; }
    bool operator!= (const Range& o) const noexcept { return min != o.min || max != o.max; }

    Val min { Val() };
    Val max { Val() };
};

/** A type of notification */
enum class Notify : uint32_t {
    NONE = 0,
    SYNC,
    ASYNC
};

namespace detail {
class Ranged;
}

/** A generic ranged Widget.
    Use  for base classes that need a min/max/value
    setup

    @see Slider,Dial
*/
class LVTK_API Ranged : public Widget {
public:
    Ranged();

    virtual ~Ranged();

    std::function<void()> on_value_changed;

    double value() const noexcept;
    void set_value (double value, Notify notify);
    void set_range (double min, double max);
    const Range<double>& range() const noexcept;

private:
    friend class detail::Ranged;
    std::unique_ptr<detail::Ranged> impl;
};

namespace detail {
class Slider;
}

/** A typical Slider control.
    Can be styled as linear bar or thumb on a track
*/
class LVTK_API Slider : public Ranged {
public:
    Slider();
    virtual ~Slider();

    enum Type : uint8_t {
        VERTICAL = 0,  ///< Vertical orientation with thumb/track
        HORIZONTAL,    ///< Horizontal orientation with thumb/track
        VERTICAL_BAR,  ///< Vertical orientation as solid bar
        HORIZONTAL_BAR ///< Horizontal orientation as solid bar
    };

    /** Change the type of slider */
    void set_type (Type type);
    /** Get the type of slider */
    Type type() const noexcept;
    bool vertical() const noexcept;

private:
    /** @private */
    bool obstructed (int x, int y) override { return true; }

    /** @private */
    void paint (Graphics& g) override;
    /** @private */
    void resized() override;
    /** @private */
    void drag (const Event&) override;
    /** @private */
    void pressed (const Event&) override;

    friend class detail::Slider;
    std::unique_ptr<detail::Slider> impl;
};

namespace detail {
class Dial;
}

/** A type of dial. Like a Knob on a synth. */
class Dial : public Ranged {
public:
    Dial();
    virtual ~Dial();

protected:
    /** @private */
    void paint (Graphics& g) override;
    /** @private */
    void resized() override;
    /** @private */
    void drag (const Event&) override;
};

} // namespace lvtk
