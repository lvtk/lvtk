// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <lvtk/lvtk.h>

namespace lvtk {
namespace detail {
class Font;
}

/** A typeface 
    @ingroup graphics
    @headerfile lvtk/ui/font.hpp
*/
class LVTK_API Typeface {
public:
    Typeface()          = default;
    virtual ~Typeface() = default;
    // clang-format off
    virtual std::string name() const noexcept =0;
    virtual const uint8_t* data() const noexcept =0;
    // clang-format on
};

/** Shared Typeface ptr.
    @ingroup graphics
    @headerfile lvtk/ui/font.hpp
 */
using TypefacePtr = std::shared_ptr<Typeface>;

/** Flag type for Font */
using FontFlags = uint8_t;

/** A font.
    @ingroup graphics
    @headerfile lvtk/ui/font.hpp
*/
class LVTK_API Font final {
public:
    enum StyleFlag : FontFlags {
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
     
        @param flags OR of @ref StyleFlag.
    */
    Font with_style (uint8_t flags) const noexcept;

    /** Duplicate this font with new height.
     
        @param height The new height
    */
    Font with_height (float height) const noexcept;

    bool operator== (const Font& o) const noexcept;
    bool operator!= (const Font& o) const noexcept;

private:
    std::shared_ptr<detail::Font> impl;
};

} // namespace lvtk
