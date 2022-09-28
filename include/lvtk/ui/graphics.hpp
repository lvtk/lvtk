// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/align.hpp>
#include <lvtk/ui/color.hpp>
#include <lvtk/ui/fill.hpp>
#include <lvtk/ui/font.hpp>
#include <lvtk/ui/image.hpp>
#include <lvtk/ui/rectangle.hpp>

namespace lvtk {

using Bounds = Rectangle<int>;

/** A 2d transformation matrix.
    @ingroup widgets
    @headerfile lvtk/ui/graphics.hpp
*/
class LVTK_API Transform final {
public:
    Transform() = default;

    bool operator== (const Transform& o) const noexcept {
        return m00 == o.m00
               && m01 == o.m01
               && m02 == o.m02
               && m10 == o.m10
               && m11 == o.m11
               && m12 == o.m12;
    }

    bool operator!= (const Transform& o) const noexcept {
        return ! operator== (o);
    }

    float m00 { 1.f }, m01 { 0.f }, m02 { 0.f },
        m10 { 0.f }, m11 { 1.f }, m12 { 0.f };
};

/** Lower level graphics context.
    @ingroup widgets
    @headerfile lvtk/ui/graphics.hpp
*/
class LVTK_API DrawingContext {
public:
    DrawingContext()          = default;
    virtual ~DrawingContext() = default;

    // clang-format off
    virtual float scale_factor() const noexcept =0;
    
    virtual void translate (const Point<int>& pt) =0;
    virtual void transform (const Transform& mat) =0;
    
    virtual void clip (const Rectangle<int>& r) =0;
    virtual void intersect_clip (const Rectangle<int>& r) =0;
    virtual Rectangle<int> last_clip() const =0;
    
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

/** Higher level graphics context.
    API is subject to change dramatically at any given time
    until we approach an alpha status.

    @ingroup widgets
    @headerfile lvtk/ui/graphics.hpp
 */
class LVTK_API Graphics final {
public:
    Graphics (DrawingContext& d);
    Graphics()  = delete;
    ~Graphics() = default;

    void translate (const Point<int>& delta);
    void clip (Bounds c);
    void intersect_clip (Bounds c);
    Bounds last_clip() const noexcept;
    bool clip_empty() const noexcept;

    void save();
    void restore();

    void set_font (const Font& font);
    void set_color (Color color);

    void fill_rect (const Rectangle<float>& r);
    void fill_rect (const Rectangle<int>& r);

    /** Draw some text */
    void draw_text (const std::string& text, Rectangle<float> area, Alignment align);

    /** Returns the context used by this Graphics instance. */
    DrawingContext& context();

private:
    DrawingContext& _context;
};

} // namespace lvtk
