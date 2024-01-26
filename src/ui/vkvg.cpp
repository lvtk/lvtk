// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <pugl/vulkan.h>
#include <vkvg.h>

#include <lvtk/ui/graphics.hpp>
#include <lvtk/ui/vulkan.hpp>

namespace lvtk {

class VulkanContext : public DrawingContext {
    VkvgDevice _device { nullptr };
    VkvgSurface _surface { nullptr };
    VkvgContext vr { nullptr };
    Bounds _last_frame {};

public:
    explicit VulkanContext() {
        stack.reserve (64);
        _device = vkvg_device_create (VK_SAMPLE_COUNT_8_BIT, false);
        auto st = vkvg_device_status (_device);

        if (VKVG_STATUS_SUCCESS != st) {
            if (_device)
                vkvg_device_destroy (_device);
            _device = nullptr;
        }
    }

    ~VulkanContext() {}

    bool have_device() const noexcept { return _device != nullptr; }

    // start a frame
    // @param frame The View size @ x=0, y=0
    // @param bounds the clip region
    bool begin_frame (lvtk::Bounds frame, lvtk::Bounds bounds) {
        state = {};
        stack.clear();

        if (_surface == nullptr || frame != _last_frame) {
            std::clog << "reframe: " << frame.str() << std::endl;
            if (vr != nullptr) {
                // std::clog << "  detatch surface\n";
                // vkvg_set_source_surface (vr, nullptr, 0, 0);
            }

            if (_surface != nullptr) {
                std::clog << "  destroy old surface\n";
                vkvg_surface_destroy (_surface);
                _surface = nullptr;
            }

            if (_surface == nullptr) {
                std::clog << "  create new source surface\n";
                _surface = vkvg_surface_create (_device, bounds.width, bounds.height);
            }
        }

        _last_frame = frame;

        if (_surface != nullptr) {
            if (vr == nullptr) {
                std::clog << "  creating context...\n";
                vr = vkvg_create (_surface);
            } else
                vkvg_set_source_surface (vr, _surface, bounds.x, bounds.y);

            clip (bounds);
            return true;
        }

        return false;
    }

    void end_frame() {
    }

    double device_scale() const noexcept override {
        double x_scale = 1.0, y_scale = 1.0;
        lvtk::ignore (x_scale, y_scale);
        // TODO:
        // if (auto s = vkvg_get_target (vr))
        //     vkvg_surface_get_device_scale (s, &x_scale, &y_scale);
        return static_cast<double> (y_scale);
    }

    void save() override {
        vkvg_save (vr);
        stack.push_back (state);
    }

    void restore() override {
        vkvg_restore (vr);
        if (stack.empty())
            return;
        std::swap (state, stack.back());
        stack.pop_back();
    }

    void set_line_width (double width) {
        vkvg_set_line_width (vr, width);
    }

    void clear_path() override {
        vkvg_new_path (vr);
    }

    void move_to (double x1, double y1) override {
        vkvg_move_to (vr, x1, y1);
    }

    void line_to (double x1, double y1) override {
        vkvg_line_to (vr, x1, y1);
    }

    void quad_to (double x1, double y1, double x2, double y2) override {
        float x0, y0;
        vkvg_get_current_point (vr, &x0, &y0);
        vkvg_curve_to (vr,
                       (x0 + 2.0 * x1) / 3.0,
                       (y0 + 2.0 * y1) / 3.0,
                       (x2 + 2.0 * x1) / 3.0,
                       (y2 + 2.0 * y1) / 3.0,
                       x2,
                       y2);
    }

    void cubic_to (double x1, double y1, double x2, double y2, double x3, double y3) override {
        vkvg_curve_to (vr, x1, y1, x2, y2, x3, y3);
    }

    void close_path() override {
        vkvg_close_path (vr);
    }

    /** Fill the current path with the currrent settings */
    void fill() override {
        apply_pending_state();
        vkvg_fill (vr);
    }

    /** Stroke the current path with current settings */
    void stroke() override {
        apply_pending_state();
        vkvg_stroke (vr);
    }

    /** Translate the origin */
    void translate (double x, double y) override {
        vkvg_translate (vr, x, y);
    }

    /** Apply transformation matrix */
    void transform (const Transform& mat) override {
        // clang-format off
        vkvg_matrix_t m = { 
            static_cast<float> (mat.m00), static_cast<float> (mat.m10),
            static_cast<float> (mat.m01), static_cast<float> (mat.m11), 
            static_cast<float> (mat.m02), static_cast<float> (mat.m12)
         };
        // clang-format on
        vkvg_transform (vr, &m);
    }

    void clip (const Rectangle<int>& r) override {
#if 1
        state.clip = r.as<double>();
        vkvg_new_path (vr);
        vkvg_rectangle (vr, r.x, r.y, r.width, r.height);
        vkvg_clip (vr);

#    if 0
        double x2, y2;
        Rectangle<double> nr;
        vkvg_clip_extents (vr, &nr.x, &nr.y, &x2, &y2);
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
        vkvg_rectangle (vr, c.x, c.y, c.width, c.height);
        vkvg_clip (vr);
        state.clip = c;
#endif
    }

    void exclude_clip (const Rectangle<int>& r) override {
        // TODO
    }

    Rectangle<int> last_clip() const override {
        return state.clip.as<int>();
    }

    Font font() const noexcept override { return state.font; }
    void set_font (const Font& f) override {
        // if (state.font == f)
        //     return;
        state.font = f;
        vkvg_set_font_size (vr, f.height());
    }

    void set_fill (const Fill& fill) override {
        auto c = state.color = fill.color();
        _fill_dirty          = false;
        vkvg_set_source_rgba (vr, c.fred(), c.fgreen(), c.fblue(), c.alpha());
    }

    void fill_rect (const Rectangle<double>& r) override {
        apply_pending_state();
        vkvg_rectangle (vr, r.x, r.y, r.width, r.height);
        vkvg_fill (vr);
    }

    FontMetrics font_metrics() const noexcept override {
        vkvg_font_extents_t cfe;
        vkvg_font_extents (vr, &cfe);
        return {
            cfe.ascent,
            cfe.descent,
            cfe.height,
            cfe.max_x_advance,
            cfe.max_y_advance
        };
    }

    TextMetrics text_metrics (std::string_view text) const noexcept override {
        vkvg_text_extents_t cte;
        vkvg_text_extents (vr, text.data(), &cte);
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
        vkvg_show_text (vr, text.data());
        return true;
    }

    void draw_image (Image i, Transform matrix) override {
        VkvgSurface image    = nullptr;
        const int bad_format = -1;
        int format           = bad_format;

        switch (i.format()) {
            case PixelFormat::ARGB32:
                format = VKVG_FORMAT_ARGB32;
                break;
            case PixelFormat::RGB24:
                format = VKVG_FORMAT_RGB24;
                break;
            case PixelFormat::INVALID:
            default:
                format = -1;
                break;
        }

        if (format == bad_format)
            return;

        image = vkvg_surface_create_from_bitmap (
            _device, i.data(), i.width(), i.height());

        if (image == nullptr)
            return;

        transform (matrix);
        vkvg_set_source_surface (vr, image, 0, 0);
        vkvg_paint (vr);

        vkvg_surface_destroy (image);
    }

private:
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
            vkvg_set_source_rgba (vr,
                                  c.fred(),
                                  c.fgreen(),
                                  c.fblue(),
                                  c.falpha());
            _fill_dirty = false;
        }
    }
};

class VkvgView : public View {
public:
    VkvgView (Main& m, Widget& w)
        : View (m, w) {
        set_backend ((uintptr_t) puglVulkanBackend());
        set_view_hint (PUGL_DOUBLE_BUFFER, PUGL_FALSE);
        set_view_hint (PUGL_RESIZABLE, PUGL_FALSE);
        auto pv = (PuglView*) c_obj();
        // TODO: make user accesible
        puglSetViewString (pv, PUGL_WINDOW_TITLE, "LVTK Vulkan Demo");
    }

    ~VkvgView() {}

    void expose (Bounds r) override {
        if (! _context_error && _context == nullptr)
            _context = std::make_unique<VulkanContext>();

        if (! _context->have_device()) {
            _context_error = true;
            _context.reset();
            std::clog << "critical error opening Vkvg context.\n";
        }

        if (! _context)
            return;

        if (_context->begin_frame (bounds().at (0), r)) {
            render (*_context);
            _context->end_frame();
        }
    }

    void created() override {
        _view    = (PuglView*) c_obj();
        _context = std::make_unique<VulkanContext>();
        assert (_view != nullptr && _context != nullptr);
    }

    void destroyed() override {
        _view = nullptr;
        _context.reset();
    }

private:
    PuglView* _view;
    std::unique_ptr<VulkanContext> _context;
    bool _context_error = false;
};

std::unique_ptr<View> Vulkan::create_view (Main& c, Widget& w) {
    return std::make_unique<VkvgView> (c, w);
}

} // namespace lvtk
