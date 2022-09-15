// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/alignment.hpp>
#include <lvtk/ui/fill.hpp>
#include <lvtk/ui/font.hpp>
#include <lvtk/ui/path.hpp>
#include <lvtk/ui/rectangle.hpp>

namespace lvtk {

class Affine final {
public:
    Affine() = default;

    bool operator== (const Affine& o) const noexcept {
        return m00 == o.m00
               && m01 == o.m01
               && m02 == o.m02
               && m10 == o.m10
               && m11 == o.m11
               && m12 == o.m12;
    }

    bool operator!= (const Affine& o) const noexcept {
        return ! operator== (o);
    }

    float m00 { 1.f }, m01 { 0.f }, m02 { 0.f },
        m10 { 0.f }, m11 { 1.f }, m12 { 0.f };
};

class Surface {
public:
    Surface()          = default;
    virtual ~Surface() = default;

    // clang-format off
    virtual float scale_factor() const noexcept =0;
    
    virtual void translate (const Point<int>& pt) =0;
    virtual void transform (const Affine& mat) =0;
    
    virtual void clip (const Rectangle<int>& r) =0;
    virtual Rectangle<int> last_clip() const =0;
    virtual void intersect_clip (const Rectangle<int>& r) =0;
    
    virtual void save() =0;
    virtual void restore() =0;
    
    virtual Font font() const noexcept =0;

    virtual void set_font (const Font& font) =0;
    virtual void set_fill (const Fill& fill) =0;

    virtual void fill_rect (const Rectangle<float>& r) =0;
    // clang-format on

    /** Draw some text.
        
        Implementations should draw the text with the current font at x/y.
        Alignment applies the the point, not the space being drawn in to.

        @param text The text to draw.
        @param x The x coordinate
        @param y The y coordinate
        @param align How align around the x/y point.
     */
    virtual bool text (const std::string& text, float x, float y, Alignment align) { return false; }
};

} // namespace lvtk
