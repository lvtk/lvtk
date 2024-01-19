// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <cairo/cairo.h>
#include <lvtk/ui/cairo.hpp>
#include <pugl/cairo.h>

namespace lvtk {

class CairoDrawingContext : public DrawingContext {
    lvtk::Color _color;

public:
    explicit CairoDrawingContext (cairo_t* obj)
        : cr (obj) {
        stack.reserve (64);
    }

    ~CairoDrawingContext() {
        cr = nullptr;
    }

    float scale_factor() const noexcept {
        double x_scale = 1.0, y_scale = 1.0;
        if (auto s = cairo_get_target (cr))
            cairo_surface_get_device_scale (s, &x_scale, &y_scale);
        return static_cast<float> (y_scale);
    }

    /** Save the current state */
    void save() override {
        cairo_save (cr);
        stack.push_back (state);
    }

    /** Restore last state */
    void restore() override {
        cairo_restore (cr);
        if (stack.empty())
            return;
        state = stack.back();
        stack.pop_back();
    }

    /** Begin a new path */
    void begin_path() override {
        cairo_new_path (cr);
    }

    /** Start a new subpath at x1 and y1 */
    void move_to (float x1, float y1) override {
        cairo_move_to (cr, x1, y1);
    }

    void line_to (float x1, float y1) override {
        cairo_line_to (cr, x1, y1);
    }

    void quad_to (float x1, float y1, float x2, float y2) override {
        double x0, y0;
        cairo_get_current_point (cr, &x0, &y0);
        cairo_curve_to (cr,
                        (x0 + 2.0 * x1) / 3.0,
                        (y0 + 2.0 * y1) / 3.0,
                        (x2 + 2.0 * x1) / 3.0,
                        (y2 + 2.0 * y1) / 3.0,
                        x2,
                        y2);
    }

    void cubic_to (float x1, float y1, float x2, float y2, float x3, float y3) override {
        cairo_curve_to (cr, x1, y1, x2, y2, x3, y3);
    }

    void close_path() override {
        cairo_close_path (cr);
    }

    /** Fill the current path with the currrent settings */
    void fill() override {
        apply_fill();
        cairo_fill (cr);
    }

    /** Stroke the current path with current settings */
    void stroke() override {
        // cairo_set_line_width (cr, 2.0);
        apply_fill();
        cairo_stroke (cr);
    }

    /** Translate the origin */
    void translate (const Point<int>& pt) override {
        // std::clog << "translate: " << pt.str() << std::endl;
        cairo_translate (cr, pt.x, pt.y);
    }

    /** Apply transformation matrix */
    void transform (const Transform& mat) override {}

    void clip (const Rectangle<int>& r) override {
        state.clip = r.as<double>();
        cairo_rectangle (cr, r.x, r.y, r.width, r.height);
        cairo_clip (cr);
    }

    void exclude_clip (const Rectangle<int>& r) override {
        // auto r2 = r.as<double>().intersection (state.clip);
        // clip (r2.as<int>());
        // cairo_rectangle (cr, r2.x, r2.y, r2.width, r2.height);
        // cairo_clip (cr);
    }

    Rectangle<int> last_clip() const override {
        return state.clip.as<int>();
    }

    /** Get the current font.
        Return the last font set with set_font
        @returns Font
    */
    Font font() const noexcept override { return state.font; }

    /** Set the current font.
        @param font Font to use for text ops.
    */
    void set_font (const Font& f) override { state.font = f; }

    /** Set the current fill type.
        Subclass should save the fill and use it for stroke/fill operations
        @param fill The new fill type to use
    */
    void set_fill (const Fill& fill) override {
        state.color = fill.color();
        // cairo_set_source_rgba (cr,
        //                        c.fred(),
        //                        c.fgreen(),
        //                        c.fblue(),
        //                        c.falpha());
        // color.r     = c.fred();
        // color.g     = c.fgreen();
        // color.b     = c.fblue();
        // color.a     = c.falpha();
    }

    void apply_fill() {
        auto c = state.color;
        cairo_set_source_rgba (cr,
                               c.fred(),
                               c.fgreen(),
                               c.fblue(),
                               c.falpha());
    }

    void fill_rect (const Rectangle<float>& r) override {
        // std::clog << "fill: " << r.str() << std::endl;
        apply_fill();
        cairo_rectangle (cr, r.x, r.y, r.width, r.height);
        cairo_fill (cr);
    }

    bool text (const std::string& text, float x, float y, Alignment align) override {
        ignore (text, x, y, align);
        return false;
    }

    void prepare (cairo_t* _cr, lvtk::Bounds bounds) {
        cr    = _cr;
        state = {};
        stack.clear();
        this->clip (bounds);
    }

private:
    cairo_t* cr { nullptr };
    struct State {
        lvtk::Color color;
        Rectangle<double> clip;
        Font font;

        State& operator= (const State& o) {
            color = o.color;
            clip  = o.clip;
            font  = o.font;
            return *this;
        }
    };

    State state;
    std::vector<State> stack;
};

class CairoView : public View {
public:
    CairoView (Main& m, Widget& w)
        : View (m, w) {
        set_backend ((uintptr_t) puglCairoBackend());
        set_view_hint (PUGL_DOUBLE_BUFFER, PUGL_FALSE);
        set_view_hint (PUGL_RESIZABLE, PUGL_TRUE);
        auto pv = (PuglView*) c_obj();
        puglSetViewString (pv, PUGL_WINDOW_TITLE, "LVTK Cairo Demo");
    }

    ~CairoView() {}

    void expose (Bounds frame) override {
        auto cr = (cairo_t*) puglGetContext ((PuglView*) c_obj());

        if (_context == nullptr)
            _context = std::make_unique<CairoDrawingContext> (cr);

        assert (_context != nullptr);
        _context->prepare (cr, frame); //{ 0, 0, bounds().width, bounds().height });
        render (*_context);
    }

    void created() override {}

    void destroyed() override {
        _context.reset();
    }

private:
    std::unique_ptr<CairoDrawingContext> _context;
};

std::unique_ptr<View> Cairo::create_view (Main& c, Widget& w) {
    return std::make_unique<CairoView> (c, w);
}

} // namespace lvtk
