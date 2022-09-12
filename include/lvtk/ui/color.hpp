// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>

namespace lvtk {

/** A 32bit color value.
    The value represented by a uint32_t. e.g. 0xRRGGBBAA
 */
class Color {
public:
    Color() = default;
    Color (uint32_t value) { pixel.value = value; }

    Color (uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        pixel.component.r = r;
        pixel.component.g = g;
        pixel.component.b = b;
        pixel.component.a = a;
    }

    Color (uint8_t r, uint8_t g, uint8_t b) {
        pixel.component.r = r;
        pixel.component.g = g;
        pixel.component.b = b;
        pixel.component.a = 0xff;
    }

    Color (float r, float g, float b, float a) {
        pixel.component.r = convert (r);
        pixel.component.g = convert (g);
        pixel.component.b = convert (b);
        pixel.component.a = convert (a);
    }

    Color (const Color& o) { pixel.value = o.pixel.value; }

    //=========================================================================
    uint8_t red() const noexcept { return pixel.component.r; }
    uint8_t green() const noexcept { return pixel.component.g; }
    uint8_t blue() const noexcept { return pixel.component.b; }
    uint8_t alpha() const noexcept { return pixel.component.a; }

    float fred() const noexcept { return (float) pixel.component.r / 255.f; }
    float fgreen() const noexcept { return (float) pixel.component.g / 255.f; }
    float fblue() const noexcept { return (float) pixel.component.b / 255.f; }
    float falpha() const noexcept { return (float) pixel.component.a / 255.f; }

    //=========================================================================
    bool operator== (const Color& o) const noexcept { return pixel.value == o.pixel.value; }
    bool operator!= (const Color& o) const noexcept { return pixel.value != o.pixel.value; }
    Color& operator= (const Color& o) {
        pixel.value = o.pixel.value;
        return *this;
    }

    Color brighter (float amount) const noexcept {
        // assert (amount >= 0.0f);
        amount = 1.0f / (1.0f + amount);
        return Color ((uint8_t) (255 - (amount * (255 - red()))),
                      (uint8_t) (255 - (amount * (255 - green()))),
                      (uint8_t) (255 - (amount * (255 - blue()))),
                      alpha());
    }

private:
    union {
        struct {
            uint8_t a, b, g, r;
        } component;
        uint32_t value { 0x000000ff };
    } pixel;

    template <typename FT>
    static inline uint8_t convert (FT v) {
        return static_cast<uint8_t> (
            (FT) 255 * std::max ((FT) 0, std::min ((FT) 1, v)));
    }
};

} // namespace lvtk
