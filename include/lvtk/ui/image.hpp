// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>

namespace lvtk {

class Pixels {
public:
};

class Image {
public:
    Image() = default;
    Image& operator= (const Image& o) { _pixels = o._pixels; return *this; }
    long use_count() const noexcept { return _pixels.use_count(); }

private:
    std::shared_ptr<Pixels> _pixels;
};

} // namespace lvtk
