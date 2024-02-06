// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#if _MSC_VER
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <cairo-win32.h>
#    undef NOMINMAX
#else
#    include <cairo/cairo.h>
#endif

#include <pugl/cairo.h>

#include <lvtk/ui/cairo.hpp>

namespace lvtk {
namespace cairo {

class Context : public DrawingContext {
public:
    explicit Context (cairo_t* context = nullptr)
        : cr (context) {
        stack.reserve (64);
    }

    ~Context() {
        cr = nullptr;
    }

    bool begin_frame (cairo_t* _cr, lvtk::Bounds bounds) {
        cr    = _cr;
        state = {};
        stack.clear();
        this->clip (bounds);
        return true;
    }

    void end_frame() {
        cr = nullptr;
    }

    double device_scale() const noexcept {
        assert (cr != nullptr);
        double x_scale = 1.0, y_scale = 1.0;
        if (auto s = cairo_get_target (cr))
            cairo_surface_get_device_scale (s, &x_scale, &y_scale);
        return static_cast<double> (y_scale);
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

    void set_line_width (double width) {
        cairo_set_line_width (cr, width);
    }

    void clear_path() override {
        cairo_new_path (cr);
    }

    void move_to (double x1, double y1) override {
        cairo_move_to (cr, x1, y1);
    }

    void line_to (double x1, double y1) override {
        cairo_line_to (cr, x1, y1);
    }

    void quad_to (double x1, double y1, double x2, double y2) override {
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

    void cubic_to (double x1, double y1, double x2, double y2, double x3, double y3) override {
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
        apply_pending_state();
        cairo_stroke (cr);
    }

    /** Translate the origin */
    void translate (double x, double y) override {
        cairo_translate (cr, x, y);
    }

    /** Apply transformation matrix */
    void transform (const Transform& mat) override {
        // clang-format off
        cairo_matrix_t m = { mat.m00, mat.m10, 
                             mat.m01, mat.m11, 
                             mat.m02, mat.m12 };
        // clang-format on
        cairo_transform (cr, &m);
    }

    void clip (const Rectangle<int>& r) override {
        state.clip = r.as<double>();
        cairo_new_path (cr);
        cairo_rectangle (cr, r.x, r.y, r.width, r.height);
        cairo_clip (cr);
    }

    void exclude_clip (const Rectangle<int>& r) override {
        // TODO
    }

    Rectangle<int> last_clip() const override {
        return state.clip.as<int>();
    }

    Font font() const noexcept override { return state.font; }
    void set_font (const Font& f) override {
        // TODO: equals operator is not yet reliable in lvtk::Font
        // if (state.font == f)
        //     return;
        state.font = f;
        cairo_set_font_size (cr, f.height());
    }

    void set_fill (const Fill& fill) override {
        auto c = state.color = fill.color();
        _fill_dirty          = false;
        cairo_set_source_rgba (cr, c.fred(), c.fgreen(), c.fblue(), c.alpha());
    }

    void fill_rect (const Rectangle<double>& r) override {
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
        cairo_show_text (cr, text.data());
        return true;
    }

    void draw_image (Image i, Transform matrix) override {
        cairo_surface_t* image = nullptr;
        cairo_format_t format  = CAIRO_FORMAT_INVALID;

        switch (i.format()) {
            case PixelFormat::ARGB32:
                format = CAIRO_FORMAT_ARGB32;
                break;
            case PixelFormat::RGB24:
                format = CAIRO_FORMAT_RGB24;
                break;
            case PixelFormat::INVALID:
            default:
                format = CAIRO_FORMAT_INVALID;
                break;
        }

        image = cairo_image_surface_create_for_data (
            i.data(), format, i.width(), i.height(), i.stride());

        if (image == nullptr)
            return;

        transform (matrix);
        cairo_set_source_surface (cr, image, 0, 0);
        cairo_paint (cr);

        cairo_surface_destroy (image);
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

class View : public lvtk::View {
public:
    View (Main& m, Widget& w)
        : lvtk::View (m, w) {
        set_backend ((uintptr_t) puglCairoBackend());
        set_view_hint (PUGL_DOUBLE_BUFFER, PUGL_FALSE);
        set_view_hint (PUGL_RESIZABLE, PUGL_TRUE);
        auto pv = (PuglView*) c_obj();
        // TODO: make user accesible
        puglSetViewString (pv, PUGL_WINDOW_TITLE, "LVTK Cairo Demo");
    }

    ~View() {}

    void expose (Bounds frame) override {
        auto cr = (cairo_t*) puglGetContext (_view);
        assert (cr != nullptr);
        if (_context->begin_frame (cr, frame)) {
            render (*_context);
            _context->end_frame();
        }
    }

    void created() override {
        _context = std::make_unique<Context>();
        _view    = (PuglView*) c_obj();
        assert (_view != nullptr && _context != nullptr);
    }

    void destroyed() override {
        _view = nullptr;
        _context.reset();
    }

private:
    using Parent = lvtk::View;
    PuglView* _view;
    std::unique_ptr<Context> _context;
};
} // namespace cairo

std::unique_ptr<lvtk::View> Cairo::create_view (Main& c, Widget& w) {
    return std::make_unique<cairo::View> (c, w);
}

} // namespace lvtk
