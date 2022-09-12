// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/image.hpp>
#include <stb/stb_image.h>

namespace lvtk {
namespace stb {

static inline uint8_t* load_file (const std::string& filename,
                                  int& width, int& height) {
    int w, h;
    int ncomponents;
    stbi_set_flip_vertically_on_load (true);
    auto image = stbi_load (filename.c_str(), &w, &h, &ncomponents, STBI_rgb_alpha);

    if (image == nullptr) {
        width = height = 0;
        return nullptr;
    }

    width = w;
    height = h;
    return image;
}

class Pixels : public lvtk::Pixels {
public:
    Pixels (int num_components)
        : _n_comps (num_components) {}

    explicit Pixels() {}
    ~Pixels() { free_data(); }

    void load_file (const char* file) {
        free_data();
        _data = stb::load_file (file, _width, _height);
    }

    int num_components() const noexcept { return _n_comps; }
    const uint8_t* data() const noexcept { return _data; }
    operator const uint8_t*() const noexcept { return _data; }
    const int width() const noexcept { return _width; }
    const int height() const noexcept { return _height; }

private:
    int _n_comps = 0;
    uint8_t* _data;
    int _width { 0 },
        _height { 0 };

    void free_data() {
        _width = _height = 0;
        if (_data)
            free (_data);
        _data = nullptr;
    }
};

} // namespace stb
} // namespace lvtk
