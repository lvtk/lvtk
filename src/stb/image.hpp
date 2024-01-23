// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <string>

#include <stb/stb_image.h>

#include <lvtk/ui/image.hpp>

namespace lvtk {
namespace stb {

/** Prepare for loading. STB doesn't seem to have a way to determine
    after loading if it is pre-multipled or not.  It does have a way
    to force unpremultiplied.  This is set true, then premultiply done
    manually.
*/
static inline void prepare_load() {
    stbi_set_flip_vertically_on_load (false);
    stbi_set_unpremultiply_on_load (true);
}

/** Converts STBI 4 channel output to premultiplied ARGB as expected by lvtk::Image. */
template<typename Size>
static inline void rgba_to_argb_and_premulitply (uint8_t* image, Size size) {
    for (uint32_t i = 0; i < static_cast<uint32_t> (size); i += 4) {
        // R-G-B-A -> B-G-R-A
        std::swap (image[i + 0], image[i + 2]);
        // pre multiply
        for (uint32_t j = i; j < i + 3; ++j)
            image[j] = (uint8_t) (float) image[j] * (float)image[i + 3] / 255.f;
    }
}

static inline uint8_t* load_memory (const uint8_t* buffer,
                                    uint32_t size,
                                    int& width, int& height, 
                                    int& num_components)
{
    prepare_load();

    auto image = stbi_load_from_memory (buffer, (int)size, &width, &height, &num_components, 4);

    if (image == nullptr || num_components <= 0 || width <= 0 || height <= 0) {
        width = height = num_components = 0;
        if (image != nullptr)
            stbi_image_free (image);
        image = nullptr;
        return nullptr;
    }

    rgba_to_argb_and_premulitply (image, 4 * width * height);
    
    return image;
}

static inline uint8_t* load_file (std::string_view filename,
                                  int& width, int& height,
                                  int& num_components) {

    prepare_load();
    
    auto image = stbi_load (filename.data(), &width, &height, &num_components, 4);

    if (image == nullptr || num_components <= 0 || width <= 0 || height <= 0) {
        width = height = num_components = 0;
        if (image != nullptr)
            stbi_image_free (image);
        image = nullptr;
        return nullptr;
    }
    
    rgba_to_argb_and_premulitply (image, 4 * width * height);

    return image;
}

class Pixels : public lvtk::Pixels {
public:
    Pixels() : lvtk::Pixels() {}
    Pixels (std::string_view filename) { 
        _data = stb::load_file (filename, _width, _height, _n_comps);
        _format = PixelFormat::ARGB32;
        if (_data !=nullptr && _n_comps > 0) {
            _stride =  ((_n_comps * std::max (1, _width) + 3) & ~3);
        } 
    }
    
    Pixels (const uint8_t* data, uint32_t size) {
        _data = stb::load_memory (data, size, _width, _height, _n_comps);
        _format = PixelFormat::ARGB32;
        if (_data !=nullptr && _n_comps > 0) {
            _stride =  ((_n_comps * std::max (1, _width) + 3) & ~3);
        }
    }

    ~Pixels() { free_data(); }

    int num_components() const noexcept { return _n_comps; }
    
    uint8_t* data() noexcept override { return _data; }
    int width() const noexcept override { return _width; }
    int height() const noexcept override { return _height; }
    int stride() const noexcept override { return _stride; }
    PixelFormat format() const noexcept override { return _format; }

private:
    uint8_t* _data { nullptr };
    PixelFormat _format;
    int _width { 0 },
        _height { 0 },
        _n_comps { 0 },
        _stride;

    void free_data() {
        _width = _height = 0;
        if (_data)
            stbi_image_free (_data);
        _data = nullptr;
    }
};

} // namespace stb
} // namespace lvtk
