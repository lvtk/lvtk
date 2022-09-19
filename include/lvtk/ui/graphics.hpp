// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/alignment.hpp>
#include <lvtk/ui/color.hpp>
#include <lvtk/ui/fill.hpp>
#include <lvtk/ui/font.hpp>
#include <lvtk/ui/image.hpp>
#include <lvtk/ui/rectangle.hpp>

namespace lvtk {

using Bounds = Rectangle<int>;

/** A 2d transformation matrix */
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

/** Lower level graphics context. */
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
 */
class LVTK_API Graphics final {
public:
    Graphics (DrawingContext& d);
    ~Graphics() = default;

    void translate (const Point<int>& delta) { surface.translate (delta); }
    void clip (Bounds c) { surface.clip (c); }
    void intersect_clip (Bounds c) { surface.intersect_clip (c); }
    Bounds last_clip() const noexcept { return surface.last_clip(); }
    bool clip_empty() const noexcept { return surface.last_clip().empty(); }

    void save() { surface.save(); }
    void restore() { surface.restore(); }

    void set_font (const Font& font);
    void set_font_height (float height);

    void set_color (Color color) { surface.set_fill (color); }

    void fill_rect (const Rectangle<float>& r) { surface.fill_rect (r); }
    void fill_rect (const Rectangle<int>& r) { surface.fill_rect (r.as<float>()); }

    /** Draw some text */
    void draw_text (const std::string& text, Rectangle<float> area, Alignment align);

    /** Returns the context used by this Graphics instance. */
    DrawingContext& context() { return surface; }

private:
    DrawingContext& surface;
};

} // namespace lvtk
