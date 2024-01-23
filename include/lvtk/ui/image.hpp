// Copyright 2022-2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>
#include <string_view>

#include <lvtk/lvtk.h>
#include <lvtk/ui/rectangle.hpp>

namespace lvtk {

enum class PixelFormat {
    INVALID = 0, ///> Invalid pixel format
    ARGB32,      ///> 32bit pre-multiplied ARGB. 0xAARRGGBB.
    RGB24        ///> 24bit RGB with the lower bits unused.
};

/** Pixel data
    @ingroup graphics
    @headerfile lvtk/ui/path.hpp
*/
class LVTK_API Pixels {
public:
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
