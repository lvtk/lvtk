// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <lvtk/lvtk.h>

namespace lvtk {

/** A typeface */
class LVTK_API Typeface {
public:
    Typeface()          = default;
    virtual ~Typeface() = default;
    // clang-format off
    virtual std::string name() const noexcept =0;
    virtual const uint8_t* data() const noexcept =0;
    // clang-format on
};

using TypefacePtr = std::shared_ptr<Typeface>;

/** A font. */
class LVTK_API Font final {
public:
    enum StyleFlag : uint8_t {
        NORMAL    = 0u,         ///< Normal font
        BOLD      = (1u << 0u), ///< Bold font
        ITALIC    = (1u << 1u), ///< Italic font
        UNDERLINE = (1u << 2u)  ///< Underline font
    };

    /** Specify a default font */
    Font();

    /** Specify a normal font with height */
    Font (float height);

    /** Specify a font with height and style flags */
    Font (uint8_t style);

    /** Specify a font with height and style flags */
    Font (float height, uint8_t style);

    ~Font() = default;

    /** Copy this font */
    Font (const Font& o);
    /** Copy this font */
    Font& operator= (const Font& o);
    /** Move this font */
    Font (Font&& o);
    /** Move this font */
    Font& operator= (Font&& o);

    /** Teturns the Typeface that should be used for this font */
    std::shared_ptr<Typeface> face() const noexcept;

    /** Returns the height of this font */
    float height() const noexcept;
    /** Returns true if this font is un-styled (normal) */
    bool normal() const noexcept;
    /** Returns true if this font is bold */
    bool bold() const noexcept;
    /** Returns true if this font is italic */
    bool italic() const noexcept;
    /** Returns true if this font is underlined */
    bool underline() const noexcept;
    /** Returns the style flags of this Font */
    uint8_t flags() const noexcept;

    /** Duplicate this font with new style flags.
     
        @param flags Or of @ref StyleFlag.
    */
    Font with_style (uint8_t flags) const noexcept;

    /** Duplicate this font with new height.
     
        @param height The new height
    */
    Font with_height (float height) const noexcept;

    bool operator== (const Font& o) const noexcept;
    bool operator!= (const Font& o) const noexcept;

private:
    static constexpr float default_height = 15.f;
    struct State {
        State() {}
        float height { default_height };
        uint8_t flags { Font::NORMAL };
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
