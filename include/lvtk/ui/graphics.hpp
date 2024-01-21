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

class Path;

/** Bounds alias Rectangle<int>
    @see Rectangle
    @ingroup graphics
    @headerfile lvtk/ui/graphics.hpp
*/
using Bounds = Rectangle<int>;

/** A 2d transformation matrix.
    @ingroup graphics
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

struct FontMetrics {
    double ascent { 0.0 };
    double descent { 0.0 };
    double height { 0.0 };
    double x_stride_max { 0.0 };
    double y_stride_max { 0.0 };
};

struct TextMetrics {
    double width { 0.0 };
    double height { 0.0 };
    double x_offset { 0.0 };
    double y_offset { 0.0 };
    double x_stride { 0.0 };
    double y_stride { 0.0 };
};

/** Lower level graphics context.
    @ingroup graphics
    @headerfile lvtk/ui/graphics.hpp
*/
class LVTK_API DrawingContext {
public:
    DrawingContext()          = default;
    virtual ~DrawingContext() = default;

    // clang-format off
    virtual float scale_factor() const noexcept =0;
    
    /** Save the current state. */
    virtual void save() =0;

    /** Restore last state. */
    virtual void restore() =0;

    /** Begin a new path */
    virtual void begin_path() =0;
    /** Start a new subpath at x1 and y1 */
    virtual void move_to (float x1, float y1) =0;
    /** Draw a line */
    virtual void line_to (float x1, float y1) =0;
    /** Draw a quadratic curve */
    virtual void quad_to (float x1, float y1, float x2, float y2) =0;
    /** Draw a cubic curve */
    virtual void cubic_to (float x1, float y1, float x2, float y2, float x3, float y3) =0;
    /** Close the current path */
    virtual void close_path() =0;

    /** Fill the current path with the currrent settings */
    virtual void fill() =0;
    /** Stroke the current path with current settings */
    virtual void stroke() =0;

    /** Translate the origin */
    virtual void translate (const Point<int>& pt) =0;
    /** Apply transformation matrix */
    virtual void transform (const Transform& mat) =0;
    
    virtual void clip (const Rectangle<int>& r) =0;
    virtual void exclude_clip (const Rectangle<int>& r) =0;
    virtual Rectangle<int> last_clip() const =0;

    /** Get the current font.
        Return the last font set with set_font
        @returns Font
    */
    virtual Font font() const noexcept =0;

    /** Set the current font.
        @param font Font to use for text ops.
    */
    virtual void set_font (const Font& font) =0;

    /** Set the current fill type.
        Subclass should save the fill and use it for stroke/fill operations
        @param fill The new fill type to use
    */
    virtual void set_fill (const Fill& fill) =0;
    
    virtual void fill_rect (const Rectangle<float>& r) =0;
    // clang-format on

    /** Returns the font metrics for the currently selected font. */
    virtual FontMetrics font_metrics() const noexcept = 0;

    /** Returns the text metrics for the currently selected font. */
    virtual TextMetrics text_metrics (std::string_view text) const noexcept = 0;

    /** Draw some text.
        
        Implementations should draw the text with the current font at x/y.
        Alignment applies the the point, not the space being drawn in to.

        @param text The text to draw.
        @param x The x coordinate.
        @param y The y coordinate.
     */
    virtual bool show_text (const std::string_view text) {
        ignore (text);
        return false;
    }
};

/** Higher level graphics context.
    API is subject to change dramatically at any given time
    until we approach an alpha status.

    @ingroup graphics
    @headerfile lvtk/ui/graphics.hpp
 */
class LVTK_API Graphics final {
public:
    Graphics (DrawingContext& d);
    Graphics()  = delete;
    ~Graphics() = default;

    /** Save the graphics state */
    void save();
    /** Restore the graphics state */
    void restore();

    /** Translate origin by delta pixels from current origin.
        @param delta delta xy to move by
     */
    void translate (const Point<int>& delta);

    /** Set the clip bounds
        @param c Bounds to set
     */
    void clip (Bounds c);

    /** Exclude a rectangle from the clip region.
        @param c Region to exclude
     */
    void exclude_clip (Bounds c);

    /** Return the last clip bounds set with clip */
    Bounds last_clip() const noexcept;

    /** Returns true if the current clip region is empty
        @returns bool
     */
    bool clip_empty() const noexcept;

    /** Set the current font */
    void set_font (const Font& font);
    void set_font (double height);

    /** Set the current fill to solid color */
    void set_color (Color color);

    void fill_path (const Path& path);

    /** Fill a rectangle with current color */
    void fill_rect (float x, float y, float width, float height);
    void fill_rect (int x, int y, int width, int height);
    void fill_rect (const Rectangle<float>& r);
    void fill_rect (const Rectangle<int>& r);

    /** Draw a rounded rectangle outline with current settings
        @param x
        @param y
        @param width
        @param height
        @param corner_size
    */
    void draw_rounded_rect (float x, float y, float width, float height, float corner_size);
    void draw_rounded_rect (int x, int y, int width, int height, float corner_size);
    void draw_rounded_rect (const Rectangle<float>&, float corner_size);
    void draw_rounded_rect (const Rectangle<int>& r, float corner_size);

    /** Fill a rounded rectangle outline with current settings
        @param x
        @param y
        @param width
        @param height
        @param corner_size
    */
    void fill_rounded_rect (float x, float y, float width, float height, float corner_size);
    void fill_rounded_rect (int x, int y, int width, int height, float corner_size);
    void fill_rounded_rect (const Rectangle<float>&, float corner_size);
    void fill_rounded_rect (const Rectangle<int>& r, float corner_size);

    /** Draw some text */
    void draw_text (const std::string& text, Rectangle<float> area, Alignment align);

    /** Returns the context used by this Graphics instance. */
    DrawingContext& context();

private:
    DrawingContext& _context;
    LVTK_DISABLE_COPY (Graphics);
};

} // namespace lvtk
