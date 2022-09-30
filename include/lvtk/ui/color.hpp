// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <algorithm>
#include <cstdint>

namespace lvtk {

/** A 32bit color value.
    The value represented by a uint32_t. e.g. 0xRRGGBBAA
    @ingroup graphics
    @headerfile lvtk/ui/color.hpp
 */
class Color {
public:
    /** Create an empty color */
    Color() = default;

    /** Create a color from uint32. The format is like a HEX color.
     
        @param value The integer value to make from.

        @code
        Color (0xff0000ff) is the same as CSS hex #ff0000ff (RED)
        @endcode
     */
    Color (uint32_t value) { pixel.value = value; }

    /** Creates a color from RGBA values (0 - 255)
        @param r Red
        @param g Green
        @param b Blue
        @param a Alpha
    */
    Color (uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        pixel.component.r = r;
        pixel.component.g = g;
        pixel.component.b = b;
        pixel.component.a = a;
    }

    /** Creates a color from RGB values (0 - 255) 100% opaque
        @param r Red
        @param g Green
        @param b Blue
    */
    Color (uint8_t r, uint8_t g, uint8_t b) {
        pixel.component.r = r;
        pixel.component.g = g;
        pixel.component.b = b;
        pixel.component.a = 0xff;
    }

    /** Creates a color from RGBA floats (0.0 - 1.0)
        @param r Red
        @param g Green
        @param b Blue
        @param a Alpha
    */
    Color (float r, float g, float b, float a) {
        pixel.component.r = convert (r);
        pixel.component.g = convert (g);
        pixel.component.b = convert (b);
        pixel.component.a = convert (a);
    }

    /** Copy this color
        @param o The other color to copy
    */
    Color (const Color& o) { pixel.value = o.pixel.value; }

    /** Return the int red value 
        @returns The red value
    */
    uint8_t red() const noexcept { return pixel.component.r; }

    /** Return the int green value
        @returns The green value
    */
    uint8_t green() const noexcept { return pixel.component.g; }

    /** Return the int blue value
        @returns the int blue value
    */
    uint8_t blue() const noexcept { return pixel.component.b; }

    /** Return the int alpha value
        @returns the int alpha value
    */
    uint8_t alpha() const noexcept { return pixel.component.a; }

    /** Return the float red value
        @returns the float red value
    */
    float fred() const noexcept { return (float) pixel.component.r / 255.f; }

    /** Return the float green value
        @returns the float green value
    */
    float fgreen() const noexcept { return (float) pixel.component.g / 255.f; }

    /** Return the float blue value
        @returns the float blue value
    */
    float fblue() const noexcept { return (float) pixel.component.b / 255.f; }

    /** Return the float alpha value
        @returns the float alpha value
    */
    float falpha() const noexcept { return (float) pixel.component.a / 255.f; }

    //=========================================================================
    bool operator== (const Color& o) const noexcept { return pixel.value == o.pixel.value; }
    bool operator!= (const Color& o) const noexcept { return pixel.value != o.pixel.value; }
    Color& operator= (const Color& o) {
        pixel.value = o.pixel.value;
        return *this;
    }

    /** Get a copy of this Color, but brighter
        @param amount How much brighter?
        @returns The brightened color
    */
    Color brighter (float amount) const noexcept {
        amount = 1.0f / (1.0f + amount);
        return Color ((uint8_t) (255 - (amount * (255.f - (float) red()))),
                      (uint8_t) (255 - (amount * (255.f - (float) green()))),
                      (uint8_t) (255 - (amount * (255.f - (float) blue()))),
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
            (FT) 255 * std::max (FT(), std::min (FT (1), v)));
    }
};

} // namespace lvtk
