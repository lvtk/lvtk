// Copyright 2022-2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>
#include <string_view>

#include <lvtk/lvtk.h>
#include <lvtk/ui/rectangle.hpp>

namespace lvtk {

/** Format for a single pixel. */
enum class PixelFormat {
    INVALID = 0, ///> Invalid format or is not supported.

    /** @brief 32-bit pixels, with alpha in the upper 8 bits, then red, then 
        green, then blue. The 32-bit quantities are stored native-endian. 
        Pre-multiplied alpha is used.
    */
    ARGB32,

    /** @brief 32-bit pixels, with the upper 8 bits unused. Red, Green, and Blue 
        are stored in the remaining 24 bits in that order. Stored native-endian.
    */
    RGB24
};

/** Pixel data
    @ingroup graphics
    @headerfile lvtk/ui/path.hpp
*/
class LVTK_API Pixels {
public:
    virtual ~Pixels() = default;

    virtual uint8_t* data() noexcept = 0;

    virtual PixelFormat format() const noexcept = 0;

    virtual int stride() const noexcept = 0;

    virtual int width() const noexcept = 0;

    virtual int height() const noexcept = 0;
};

/** An image
    @ingroup graphics
    @headerfile lvtk/ui/path.hpp
*/
class LVTK_API Image {
public:
    Image() = default;
    Image& operator= (const Image& o) {
        _pixels = o._pixels;
        return *this;
    }

    /** Load from file. */
    static Image load (std::string_view filename);

    /** Load by data.  Data can be PNG, JPG. */
    static Image load (const uint8_t* data, uint32_t size);

    PixelFormat format() const noexcept {
        return _pixels != nullptr ? _pixels->format() : PixelFormat::ARGB32;
    }

    Rectangle<int> bounds() const noexcept { return { 0, 0, width(), height() }; }
    int stride() const noexcept { return _pixels != nullptr ? _pixels->stride() : 0; }
    int width() const noexcept { return _pixels != nullptr ? _pixels->width() : 0; }
    int height() const noexcept { return _pixels != nullptr ? _pixels->height() : 0; }

    uint8_t* data() noexcept { return _pixels != nullptr ? _pixels->data() : nullptr; }
    bool valid() const noexcept;
    long use_count() const noexcept { return _pixels.use_count(); }
    operator bool() const noexcept { return valid(); }

private:
    std::shared_ptr<Pixels> _pixels;
};

} // namespace lvtk
