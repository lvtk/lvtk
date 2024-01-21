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
    explicit CairoDrawingContext (cairo_t* context = nullptr)
        : cr (context) {
        stack.reserve (64);
    }

    ~CairoDrawingContext() {
        cr = nullptr;
    }

    void begin_frame (cairo_t* _cr, lvtk::Bounds bounds) {
        cr    = _cr;
        state = {};
        stack.clear();
        this->clip (bounds);
    }

    void end_frame() {
        cr = nullptr;
    }

    float scale_factor() const noexcept {
        double x_scale = 1.0, y_scale = 1.0;
        if (auto s = cairo_get_target (cr))
            cairo_surface_get_device_scale (s, &x_scale, &y_scale);
        return static_cast<float> (y_scale);
    }

    void save() override {
        cairo_save (cr);
        stack.push_back (state);
    }

    void restore() override {
        cairo_restore (cr);
        if (stack.empty())
            return;
        std::swap (state, stack.back());
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
        apply_pending_state();
        cairo_fill (cr);
    }

    /** Stroke the current path with current settings */
    void stroke() override {
        // cairo_set_line_width (cr, 2.0);
        apply_pending_state();
        cairo_stroke (cr);
    }

    /** Translate the origin */
    void translate (const Point<int>& pt) override {
        cairo_translate (cr, pt.x, pt.y);
    }

    /** Apply transformation matrix */
    void transform (const Transform& mat) override {
        cairo_matrix_t m;
        cairo_matrix_init (&m, mat.m00, mat.m01, mat.m02, mat.m10, mat.m11, mat.m12);
        cairo_transform (cr, &m);
        // TODO
    }

    void clip (const Rectangle<int>& r) override {
#if 1
        state.clip = r.as<double>();
        cairo_new_path (cr);
        cairo_rectangle (cr, r.x, r.y, r.width, r.height);
        cairo_clip (cr);

#    if 0
        double x2, y2;
        Rectangle<double> nr;
        cairo_clip_extents (cr, &nr.x, &nr.y, &x2, &y2);
        nr.width  = x2 - nr.x;
        nr.height = y2 - nr.y;

        if (nr != state.clip) {
            std::clog << "RA: " << state.clip.str() << std::endl
                << "RB: " << nr.str() << std::endl;
            state.clip = nr;
        }
#    endif
#else
        auto c = state.clip.empty() ? r.as<double>()
                                    : state.clip.intersection (r.as<double>());
        cairo_rectangle (cr, c.x, c.y, c.width, c.height);
        cairo_clip (cr);
        state.clip = c;
#endif
    }

    void exclude_clip (const Rectangle<int>& r) override {
        // TODO
    }

    // Rectangle<double> clip_extents() const noexcept {
    //     // Rectangle<double> c;
    //     // double x2, y2;
    //     // cairo_clip_extents (cr, &c.x, &c.y, &x2, &y2);
    //     // c.width = x2 - c.x;
    //     // c.height = y2 - c.y;
    //     // state.clip = c;
    //     return state.clip.as<int>();
    // }

    Rectangle<int> last_clip() const override {
        return state.clip.as<int>();
    }

    Font font() const noexcept override { return state.font; }
    void set_font (const Font& f) override {
        // if (state.font == f)
        //     return;
        state.font = f;
        cairo_set_font_size (cr, f.height());
    }

    void set_fill (const Fill& fill) override {
        state.color = fill.color();
        _fill_dirty = true;
    }

    void fill_rect (const Rectangle<float>& r) override {
        apply_pending_state();
        cairo_rectangle (cr, r.x, r.y, r.width, r.height);
        cairo_fill (cr);
    }

    FontMetrics font_metrics() const noexcept override {
        cairo_font_extents_t cfe;
        cairo_font_extents (cr, &cfe);
        return {
            cfe.ascent,
            cfe.descent,
            cfe.height,
            cfe.max_x_advance,
            cfe.max_y_advance
        };
    }

    TextMetrics text_metrics (std::string_view text) const noexcept override {
        cairo_text_extents_t cte;
        cairo_text_extents (cr, text.data(), &cte);
        return {
            cte.width,
            cte.height,
            cte.x_bearing,
            cte.y_bearing,
            cte.x_advance,
            cte.y_advance
        };
    }

    bool show_text (std::string_view text) override {
        apply_pending_state();
        // cairo_text_extents_t extents;
        // cairo_set_font_size (cr, state.font.height());
        // cairo_text_extents (cr, text.c_str(), &extents);
        // cairo_move_to (cr, x, y);
        cairo_show_text (cr, text.data());
        return true;
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

    bool _fill_dirty = false;

    void apply_pending_state() {
        if (_fill_dirty) {
            auto c = state.color;
            cairo_set_source_rgba (cr,
                                   c.fred(),
                                   c.fgreen(),
                                   c.fblue(),
                                   c.falpha());
            _fill_dirty = false;
        }
    }
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
        auto cr = (cairo_t*) puglGetContext (_view);
        assert (cr != nullptr);
        _context->begin_frame (cr, frame);
        render (*_context);
        _context->end_frame();
    }

    void created() override {
        _context = std::make_unique<CairoDrawingContext>();
        _view    = (PuglView*) c_obj();
        assert (_view != nullptr && _context != nullptr);
    }

    void destroyed() override {
        _view = nullptr;
        _context.reset();
    }

private:
    PuglView* _view;
    std::unique_ptr<CairoDrawingContext> _context;
};

std::unique_ptr<View> Cairo::create_view (Main& c, Widget& w) {
    return std::make_unique<CairoView> (c, w);
}

} // namespace lvtk
