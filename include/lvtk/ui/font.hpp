// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace lvtk {

/** A typeface */
class Typeface {
public:
    Typeface()          = default;
    virtual ~Typeface() = default;
    // clang-format off
    virtual const uint8_t* data() const noexcept =0;
    // clang-format on
};

using TypefacePtr = std::shared_ptr<Typeface>;

/** Style flags for a Font. */
struct FontStyle {
    enum : uint32_t {
        NORMAL    = 0u,          ///< Normal font
        BOLD      = (1u << 0u), ///< Bold font
        ITALIC    = (1u << 1u), ///< Italic font
        UNDERLINE = (1u << 2u)  ///< Underline font
    };
};

/** A font. */
class Font final {
public:
    /** Specify a default font */
    Font() : _state (std::make_shared<State>()) {}

    /** Specify a normal font with height */
    Font (float height) : _state (std::make_shared<State>()) {
        _state->height = height;
        _state->flags  = FontStyle::NORMAL;
    }

    /** Specify a font with height and style flags */
    Font (uint32_t style) : _state (std::make_shared<State>()) {
        _state->flags  = style;
    }

    /** Specify a font with height and style flags */
    Font (float height, uint32_t style) : _state (std::make_shared<State>()) {
        _state->height = height;
        _state->flags  = style;
    }

    ~Font() = default;

    Font (const Font& o) : _state (o._state) {}
    Font& operator= (const Font& o) {
        _state = o._state;
        return *this;
    }

    Font (Font&& o) : _state (std::move (o._state)) {}
    Font& operator= (Font&& o) {
        _state = std::move (o._state);
        return *this;
    }

    /** Teturns the Typeface that should be used for this font */
    std::shared_ptr<Typeface> face() const noexcept { return _state->face; }

    /** Returns the height of this font */
    float height() const noexcept { return _state->height; }
    /** Returns true if this font is un-styled (normal) */
    bool normal() const noexcept { return _state->flags == FontStyle::NORMAL; }
    /** Returns true if this font is bold */
    bool bold() const noexcept { return (_state->flags & FontStyle::BOLD) != 0; }
    /** Returns true if this font is italic */
    bool italic() const noexcept { return (_state->flags & FontStyle::ITALIC) != 0; }
    /** Returns true if this font is underlined */
    bool underline() const noexcept { return (_state->flags & FontStyle::UNDERLINE) != 0; }

    uint8_t flags() const noexcept { return _state->flags; }

    /** Duplicate this font with new style flags.
     
        @param style The StyleFlag's to use.
    */
    Font with_style (uint8_t flags) {
        Font f;
        *f._state       = *_state;
        f._state->flags = flags;
        return f;
    }

    /** Duplicate this font with new height
     
        @param height The new height
    */
    Font with_height (float height) const noexcept {
        Font f;
        *f._state        = *_state;
        f._state->height = height;
        return f;
    }

    bool operator== (const Font& o) const noexcept {
        return _state == o._state || *_state == *o._state;
    }
    bool operator!= (const Font& o) const noexcept {
        return _state != o._state || *_state != *o._state;
    }

private:
    static constexpr float default_height = 15.f;
    struct State {
        State() {}
        float height { default_height };
        uint32_t flags { FontStyle::NORMAL };
        std::shared_ptr<Typeface> face;
        State (const State& o) { operator= (o); }
        State& operator= (const State& o) {
            height = o.height;
            flags  = o.flags;
            face   = o.face;
            return *this;
        }
        bool operator== (const State& o) const noexcept {
            return height == o.height && flags == o.flags && face == o.face;
        }
        bool operator!= (const State& o) const noexcept {
            return ! operator== (o);
        }
    };
    std::shared_ptr<State> _state;
};

} // namespace lvtk
