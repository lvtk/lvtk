#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/widget.hpp>

namespace lvtk {

template <typename Val>
class Range {
public:
    Range() = default;
    Val min { Val() };
    Val max { Val() };
};

namespace detail {
class Slider;
}

namespace detail {
class Ranged;
}
class Ranged : public Widget {
public:
    Ranged();

    virtual ~Ranged();

    double value() const noexcept { return _value; }
    void set_value (double value) { _value = value; }

    void set_range (double min, double max) {
        _range.min = min;
        _range.max = max;
    }

    double min_value() const noexcept { return _range.min; }
    double max_value() const noexcept { return _range.max; }

    Range<double> range() const noexcept { return _range; }

private:
    Range<double> _range;
    double _value;
};

class LVTK_API Slider : public Ranged {
public:
    Slider();
    virtual ~Slider();

private:
    bool obstructed (int x, int y) override {
        return true;
    }

    /** @private */
    void paint (Graphics& g) override;
    /** @private */
    void resized() override;
    /** @private */
    void drag (const Event&) override;

    friend class detail::Slider;
    std::unique_ptr<detail::Slider> impl;
};

namespace detail {
class Dial;
}
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

namespace detail {
class ImageDial;
}
class ImageDial : public Dial {
public:
    ImageDial();
    virtual ~ImageDial();

protected:
    /** @private */
    void paint (Graphics& g) override;
    /** @private */
    void resized() override;
    /** @private */
    void drag (const Event&) override;
};

} // namespace lvtk
