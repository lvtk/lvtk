// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/image.hpp>

#include "stb/image.hpp"

namespace lvtk {
namespace detail {
static inline bool pixels_valid (const Pixels& pix) {
    return pix.format() != PixelFormat::INVALID && pix.height() > 0 && pix.width() > 0 && pix.stride() > 0;
}
} // namespace detail

Image Image::load (std::string_view filename) {
    Image i;

    if (auto pix = std::make_shared<stb::Pixels> (filename))
        if (detail::pixels_valid (*pix))
            i._pixels = pix;

    return i;
}

Image Image::load (const uint8_t* data, uint32_t size) {
    Image i;
    if (auto pix = std::make_shared<stb::Pixels> (data, size))
        if (detail::pixels_valid (*pix))
            i._pixels = pix;
    return i;
}

bool Image::valid() const noexcept {
    return _pixels != nullptr;
}

} // namespace lvtk

#include "stb/image.ipp"
