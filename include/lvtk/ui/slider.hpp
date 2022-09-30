// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <functional>

#include <lvtk/lvtk.h>
#include <lvtk/ui/notify.hpp>
#include <lvtk/ui/widget.hpp>

// clang-format off
namespace lvtk { namespace detail { class Ranged; } }
namespace lvtk { namespace detail { class Slider; } }
namespace lvtk { namespace detail { class Dial; } }
// clang-format on

namespace lvtk {

/** A range of two values
    E.g. min and max with some conversion helpers
    @ingroup widgets
    @headerfile lvtk/ui/slider.hpp
    @tparam Val the value type
 */
template <typename Val>
class Range {
public:
    /** Minimum value */
    Val min { Val() };
    /** Maximum value */
    Val max { Val() };

    /** Make an empty range */
    Range() = default;

    /** Make a range with min and max 
        @param minimum min Value
        @param maximum max Value
    */
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
};

/** A generic ranged Widget.
    Use  for base classes that need a min/max/value
    setup

    @ingroup widgets
    @headerfile lvtk/ui/slider.hpp
    @see Slider,Dial
*/
class LVTK_API Ranged : public Widget {
public:
    /** Create a new ranged */
    Ranged();

    virtual ~Ranged();

    /** Called when the value changes */
    std::function<void()> on_value_changed;

    /** @returns the current value */
    double value() const noexcept;

    /** Set the current value
        @param value New value
        @param notify How to notify 
    */
    void set_value (double value, Notify notify);

    /** Set the min/max range
        @param min Min value
        @param max Max value
    */
    void set_range (double min, double max);

    /** Get the Range object used
        @returns the range
    */
    const Range<double>& range() const noexcept;

private:
    friend class detail::Ranged;
    std::unique_ptr<detail::Ranged> impl;
};

/** A typical Slider control.
    Can be styled as linear bar or thumb on a track

    @ingroup widgets
    @headerfile lvtk/ui/slider.hpp
*/
class LVTK_API Slider : public Ranged {
public:
    Slider();
    virtual ~Slider();

    /** The type of slider */
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
    /** Returns true if this has vertical orientation */
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

/** A type of dial. Like a Knob on a synth.
    @ingroup widgets
    @headerfile lvtk/ui/slider.hpp
*/
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
