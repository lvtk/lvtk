// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/font.hpp>

namespace lvtk {
namespace detail {

static constexpr float default_height = 15.f;

class Font {
public:
    Font() {}
    float height { default_height };
    uint8_t flags { lvtk::Font::NORMAL };
    std::shared_ptr<Typeface> face;
    Font (const Font& o) { operator= (o); }
    Font& operator= (const Font& o) {
        height = o.height;
        flags  = o.flags;
        face   = o.face;
        return *this;
    }
    bool operator== (const Font& o) const noexcept {
        return height == o.height && flags == o.flags && face == o.face;
    }
    bool operator!= (const Font& o) const noexcept {
        return ! operator== (o);
    }
};

} // namespace detail

Font::Font() : impl (std::make_shared<detail::Font>()) {}

Font::Font (float height) : impl (std::make_shared<detail::Font>()) {
    impl->height = height;
    impl->flags  = Font::NORMAL;
}

Font::Font (uint8_t style) : impl (std::make_shared<detail::Font>()) {
    impl->flags = style;
}

Font::Font (float height, uint8_t style) : impl (std::make_shared<detail::Font>()) {
    impl->height = height;
    impl->flags  = style;
}

Font::Font (const Font& o) : impl (o.impl) {}
Font& Font::operator= (const Font& o) {
    impl = o.impl;
    return *this;
}

Font::Font (Font&& o) : impl (std::move (o.impl)) {}
Font& Font::operator= (Font&& o) {
    impl = std::move (o.impl);
    return *this;
}

std::shared_ptr<Typeface> Font::face() const noexcept { return impl->face; }
float Font::height() const noexcept { return impl->height; }
bool Font::normal() const noexcept { return impl->flags == NORMAL; }
bool Font::bold() const noexcept { return (impl->flags & BOLD) != 0; }
bool Font::italic() const noexcept { return (impl->flags & ITALIC) != 0; }
bool Font::underline() const noexcept { return (impl->flags & UNDERLINE) != 0; }
uint8_t Font::flags() const noexcept { return impl->flags; }

Font Font::with_style (uint8_t flags) const noexcept {
    Font f;
    *f.impl       = *impl;
    f.impl->flags = flags;
    return f;
}

Font Font::with_height (float height) const noexcept {
    Font f;
    *f.impl        = *impl;
    f.impl->height = height;
    return f;
}

bool Font::operator== (const Font& o) const noexcept {
    return impl == o.impl || *impl == *o.impl;
}

bool Font::operator!= (const Font& o) const noexcept {
    return impl != o.impl || *impl != *o.impl;
}

} // namespace lvtk
