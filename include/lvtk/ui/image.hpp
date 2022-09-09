// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

namespace lvtk {

class Pixels {
public:
};

class Image {
public:
    Image() = default;
    Image& operator= (const Image& o) { return *this; }
    long use_count() const noexcept {_pixels.use_count(); }

private:
    std::shared_ptr<Pixels> _pixels;
};

} // namespace lvtk
