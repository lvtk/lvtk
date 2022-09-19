// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/font.hpp>

namespace lvtk {

Font::Font() : _state (std::make_shared<State>()) {}

Font::Font (float height) : _state (std::make_shared<State>()) {
    _state->height = height;
    _state->flags  = Font::NORMAL;
}

Font::Font (uint8_t style) : _state (std::make_shared<State>()) {
    _state->flags = style;
}

Font::Font (float height, uint8_t style) : _state (std::make_shared<State>()) {
    _state->height = height;
    _state->flags  = style;
}

Font::Font (const Font& o) : _state (o._state) {}
Font& Font::operator= (const Font& o) {
    _state = o._state;
    return *this;
}

Font::Font (Font&& o) : _state (std::move (o._state)) {}
Font& Font::operator= (Font&& o) {
    _state = std::move (o._state);
    return *this;
}

std::shared_ptr<Typeface> Font::face() const noexcept { return _state->face; }
float Font::height() const noexcept { return _state->height; }
bool Font::normal() const noexcept { return _state->flags == NORMAL; }
bool Font::bold() const noexcept { return (_state->flags & BOLD) != 0; }
bool Font::italic() const noexcept { return (_state->flags & ITALIC) != 0; }
bool Font::underline() const noexcept { return (_state->flags & UNDERLINE) != 0; }
uint8_t Font::flags() const noexcept { return _state->flags; }

Font Font::with_style (uint8_t flags) const noexcept {
    Font f;
    *f._state       = *_state;
    f._state->flags = flags;
    return f;
}

Font Font::with_height (float height) const noexcept {
    Font f;
    *f._state        = *_state;
    f._state->height = height;
    return f;
}

bool Font::operator== (const Font& o) const noexcept {
    return _state == o._state || *_state == *o._state;
}

bool Font::operator!= (const Font& o) const noexcept {
    return _state != o._state || *_state != *o._state;
}

} // namespace lvtk
