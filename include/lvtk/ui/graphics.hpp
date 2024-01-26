// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/color.hpp>
#include <lvtk/ui/fill.hpp>
#include <lvtk/ui/fitment.hpp>
#include <lvtk/ui/font.hpp>
#include <lvtk/ui/image.hpp>
#include <lvtk/ui/justify.hpp>
#include <lvtk/ui/rectangle.hpp>
#include <lvtk/ui/transform.hpp>

namespace lvtk {

class Path;

/** A scoped save & restore helper. The constructor calls `save()`, the 
    destructor calls `restore()`
*/
template <class Save>
struct ScopedSave {
    using save_type = Save;
    ScopedSave()    = delete;

    /** Save without arguments */
    ScopedSave (Save& s) noexcept : _ref (s) { _ref.save(); }

    /** Save with arguments */
    template<typename ...Args>
    ScopedSave (Save& s, Args&& ...args) 
        : _ref(s) { _ref.save (std::forward<Args>(args)...); }
    ~ScopedSave() noexcept { _ref.restore(); }

private:
    Save& _ref;
    LVTK_DISABLE_COPY (ScopedSave)
    LVTK_DISABLE_MOVE (ScopedSave)
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
    virtual double device_scale() const noexcept =0;
    
    /** Save the current state. */
    virtual void save() =0;

    /** Restore last state. */
    virtual void restore() =0;

    /** Set the line width. */
    virtual void set_line_width (double width) =0;

    /** Clears the current path. After calling there will be no path and no 
        current position.
     */
    virtual void clear_path() =0;

    /** Begin a new sub-path. After calling the current point will be (x, y). */
    virtual void move_to (double x, double y) =0;
    
    /** Draw a line */
    virtual void line_to (double x1, double y1) =0;
    
    /** Draw a quadratic curve */
    virtual void quad_to (double x1, double y1, double x2, double y2) =0;
    
    /** Adds a cubic Bézier curve to the path from the current point to 
        position (x3 , y3) in user-space coordinates. The (x1, y1) and 
        (x2, y2) are used as the control points. After returning current point 
        will be (x3, y3).

        If there is no current point before the call to cubic_to() this 
        function will behave as if preceded by calling move_to (x1, y1).
    */
    virtual void cubic_to (double x1, double y1, double x2, double y2, double x3, double y3) =0;
    
    /** Close the current path */
    virtual void close_path() =0;

    /** Fill the current path with the currrent settings */
    virtual void fill() =0;
    
    /** Stroke the current path with current settings */
    virtual void stroke() =0;

    /** Translate the origin */
    virtual void translate (double dx, double dy) =0;

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
    
    virtual void fill_rect (const Rectangle<double>& r) =0;
    // clang-format on

    /** Returns the font metrics for the currently selected font. */
    virtual FontMetrics font_metrics() const noexcept = 0;

    /** Returns the text metrics for the currently selected font. */
    virtual TextMetrics text_metrics (std::string_view text) const noexcept = 0;

    /** Draw some text.
        
        Implementations should draw the text with the current font at x/y.
        Justify applies the the point, not the space being drawn in to.

        @param text The text to draw.
        @param x The x coordinate.
        @param y The y coordinate.
     */
    virtual bool show_text (const std::string_view text) {
        ignore (text);
        return false;
    }

    virtual void draw_image (Image image, Transform transform) {
        lvtk::ignore (image, transform);
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

    /** Returns the context used by this Graphics instance. */
    DrawingContext& context();

    /** Save the graphics state */
    void save();

    /** Restore the graphics state */
    void restore();

    /** Translate origin by delta pixels from current origin.
        @param delta delta xy to move by
     */
    void translate (Point<int> delta);

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

    void stroke_path (const Path& path);

    /** Draw some text */
    void draw_text (const std::string& text, Rectangle<float> area, Justify align);

    /** Draw an image. */
    void draw_image (Image image, Rectangle<double> target, Fitment align);
    void draw_image (Image image, Transform transform);

private:
    DrawingContext& _context;
    LVTK_DISABLE_COPY (Graphics)
};

} // namespace lvtk
