// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>
#include <unordered_map>
#include <vector>

#include <lvtk/ui/font.hpp>

#include "gl.hpp"

#include "../nanovg/nanovg.h"
#include "../nanovg/nanovg_gl.h"
#include "Roboto-Bold.ttf.h"
#include "Roboto-Regular.ttf.h"
#include "nanovg.hpp"

namespace lvtk {
namespace nvg {
namespace detail {

static constexpr const char* default_font_face      = "Roboto-Normal";
static constexpr const char* default_font_face_bold = "Roboto-Bold";

#if defined(NANOVG_GL2)
static constexpr auto create  = nvgCreateGL2;
static constexpr auto destroy = nvgDeleteGL2;
#elif defined(NANOVG_GL3)
static constexpr auto create  = nvgCreateGL3;
static constexpr auto destroy = nvgDeleteGL3;
#else
#    error "No GL version specified for NanoVG"
#endif

static uint64_t hash (uint8_t* data, size_t size) {
    unsigned long hash = 5381;
    int c;

    for (size_t i = 0; i < size; ++i) {
        c    = data[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

} // namespace detail

namespace convert {
#if 0 // save, ignore unused warning
static int alignment (Justify align) {
    uint32_t flags = 0;

    if ((align.flags() & Justify::LEFT) != 0)
        flags |= NVG_ALIGN_LEFT;
    if ((align.flags() & Justify::RIGHT) != 0)
        flags |= NVG_ALIGN_RIGHT;
    if ((align.flags() & Justify::CENTER) != 0)
        flags |= NVG_ALIGN_CENTER;
    if ((align.flags() & Justify::TOP) != 0)
        flags |= NVG_ALIGN_TOP;
    if ((align.flags() & Justify::BOTTOM) != 0)
        flags |= NVG_ALIGN_BOTTOM;
    if ((align.flags() & Justify::MIDDLE) != 0)
        flags |= NVG_ALIGN_MIDDLE;

    return static_cast<int> (flags);
};
#endif
} // namespace convert

class Context::Ctx {
    int _font_normal = 0;
    int _font_bold   = 0;

public:
    Ctx() : ctx (detail::create (NVG_ANTIALIAS | NVG_STENCIL_STROKES)) {
        _font_normal = nvgCreateFontMem (ctx,
                                         detail::default_font_face,
                                         (uint8_t*) Roboto_Regular_ttf,
                                         Roboto_Regular_ttf_size,
                                         0);
        _font_bold   = nvgCreateFontMem (ctx,
                                       detail::default_font_face_bold,
                                       (uint8_t*) Roboto_Bold_ttf,
                                       Roboto_Bold_ttf_size,
                                       0);
    }

    ~Ctx() {
        if (ctx)
            detail::destroy (ctx);
    }

    void save() {
        stack.push_back (state);
        // std::clog << "save: clip:    " << state.clip.str() << std::endl;
        nvgSave (ctx);
    }

    void restore() {
        nvgRestore (ctx);
        if (! stack.empty()) {
            state = stack.back();
            // std::clog << "restore: clip: " << state.clip.str() << std::endl;
            stack.pop_back();
        }
    }

    struct FontExtent {
        float ascent { 0 };
        float descent { 0 };
        float height { 0 };
    };

    FontExtent font_extent() {
        FontExtent fe;
        nvgTextMetrics (ctx, &fe.ascent, &fe.descent, &fe.height);
        return fe;
    }

    Rectangle<float> text_area (const char* str) {
        Rectangle<float> r;
        float bounds[4] = { 0 };
        nvgTextBoxBounds (ctx, 0.f, 0.f, 1000.f, str, nullptr, bounds);
        r.x      = bounds[0];
        r.y      = bounds[1];
        r.width  = bounds[2] - r.x;
        r.height = bounds[3] = r.y;
        return r;
    }

    void reset() {
        nvgReset (ctx);
        last_pos.x = last_pos.y = 0;
    }

    auto image_hash (Image i) {
        return detail::hash (i.data(), i.width() * i.height() * 4);
    }

    void add_image (uint64_t key, int handle) { images[key] = handle; }
    int find_image (uint64_t key) { return images[key]; }

private:
    friend class nvg::Context;
    NVGcontext* ctx { nullptr };

    std::unordered_map<uint64_t, int> images;

    Point<float> last_pos;

    struct State {
        NVGcolor color;
        Rectangle<float> clip;
        Font font;
        int font_id = 0;

        State& operator= (const State& o) {
            color   = o.color;
            clip    = o.clip;
            font    = o.font;
            font_id = o.font_id;
            return *this;
        }
    };

    float internal_scale = 1.f;
    State state;
    std::vector<State> stack;
};

Context::Context()
    : ctx (std::make_unique<Ctx>()) {
}

Context::~Context() {
    for (const auto image : ctx->images)
        nvgDeleteImage (ctx->ctx, image.second);
    ctx->images.clear();
    ctx.reset();
}

void Context::begin_frame (int width, int height, double scale) {
    ctx->internal_scale = scale;
    ctx->stack.clear();
    ctx->state = {};
    nvgBeginFrame (ctx->ctx,
                   (float) width,
                   (float) height,
                   ctx->internal_scale);

    clip ({ 0, 0, width, height });
    nvgStrokeWidth (ctx->ctx, 2);
    nvgPathWinding (ctx->ctx, NVG_CCW);
}

void Context::end_frame() {
    nvgEndFrame (ctx->ctx);
}

double Context::scale_factor() const noexcept {
    return ctx->internal_scale;
}

void Context::translate (double x, double y) {
    nvgTranslate (ctx->ctx,
                  static_cast<float> (x),
                  static_cast<float> (y));
}

// [a c e]
// [b d f]
// [0 0 1]
void Context::transform (const Transform& mat) {
    // nvgTransform (ctx->ctx, mat.m00, mat.m01, mat.m02,
    //                         mat.m10, mat.m11, mat.m12);
    nvgTransform (ctx->ctx, mat.m00, mat.m10, mat.m01, mat.m11, mat.m02, mat.m12);
}

void Context::set_line_width (double width) {
    nvgStrokeWidth (ctx->ctx, static_cast<float> (width));
}

void Context::clear_path() {
    ctx->last_pos = {};
    nvgBeginPath (ctx->ctx);
}

void Context::move_to (double x1, double y1) {
    ctx->last_pos.x = x1;
    ctx->last_pos.y = y1;
    nvgMoveTo (ctx->ctx, x1, y1);
}

void Context::line_to (double x1, double y1) {
    ctx->last_pos.x = x1;
    ctx->last_pos.y = y1;
    nvgLineTo (ctx->ctx, x1, y1);
}

void Context::quad_to (double x1, double y1, double x2, double y2) {
    ctx->last_pos.x = x2;
    ctx->last_pos.y = y2;
    nvgQuadTo (ctx->ctx, x1, y1, x2, y2);
}

void Context::cubic_to (double x1, double y1, double x2, double y2, double x3, double y3) {
    ctx->last_pos.x = x3;
    ctx->last_pos.y = y3;
    nvgBezierTo (ctx->ctx, x1, y1, x2, y2, x3, y3);
}

void Context::close_path() {
    nvgClosePath (ctx->ctx);
}

void Context::fill() {
    nvgFill (ctx->ctx);
    clear_path();
}

void Context::stroke() {
    nvgStroke (ctx->ctx);
    clear_path();
}

void Context::clip (const Rectangle<int>& r) {
#if 1
    ctx->state.clip = r.as<float>();
    nvgScissor (ctx->ctx,
                ctx->state.clip.x,
                ctx->state.clip.y,
                ctx->state.clip.width,
                ctx->state.clip.height);
#else
    auto c = ctx->state.clip.empty() ? r.as<float>()
                                     : ctx->state.clip.intersection (r.as<float>());
    nvgScissor (ctx->ctx, c.x, c.y, c.width, c.height);
    ctx->state.clip = c;
#endif
}

void Context::exclude_clip (const Rectangle<int>& r) {
    // noop
    return;
}

Rectangle<int> Context::last_clip() const {
    return (ctx->state.clip).as<int>();
}

Font Context::font() const noexcept { return ctx->state.font; }
void Context::set_font (const Font& font) {
    ctx->state.font    = font;
    ctx->state.font_id = font.bold() ? ctx->_font_bold : ctx->_font_normal;
    nvgFontSize (ctx->ctx, ctx->state.font.height());
    nvgFontFaceId (ctx->ctx, ctx->state.font_id);
}

void Context::set_fill (const Fill& fill) {
    auto c      = fill.color();
    auto& color = ctx->state.color;
    color.r     = c.fred();
    color.g     = c.fgreen();
    color.b     = c.fblue();
    color.a     = c.falpha();

    nvgStrokeColor (ctx->ctx, color);
    nvgFillColor (ctx->ctx, color);
    // nvgFillPaint (ctx->ctx, 0);
}

void Context::save() { ctx->save(); }
void Context::restore() { ctx->restore(); }

void Context::fill_rect (const Rectangle<double>& r) {
    nvgBeginPath (ctx->ctx);

    nvgRect (ctx->ctx,
             (r.x),
             (r.y),
             r.width,
             r.height);

    fill();
}

FontMetrics Context::font_metrics() const noexcept {
    float a, d, lh;
    nvgTextMetrics (ctx->ctx, &a, &d, &lh);
    return { a, d, lh, 0.0, 0.0 };
}

TextMetrics Context::text_metrics (std::string_view text) const noexcept {
    lvtk::Point<float> pt { 0.f, 0.f };
    TextMetrics te;
    float b[4] = { 0.f };
    float ascent, descent, lineheight;

    nvgTextMetrics (ctx->ctx, &ascent, &descent, &lineheight);
    te.x_stride = nvgTextBounds (ctx->ctx, pt.x, pt.y, text.data(), nullptr, b);
    te.width    = b[2] - b[0];
    te.height   = b[3] - b[1];
    te.x_offset = 0;
    te.y_offset = ascent + descent - lineheight;
    te.y_stride = 0;

    return te;
}

bool Context::show_text (std::string_view text) {
    nvgSave (ctx->ctx);
    nvgTextAlign (ctx->ctx, NVG_ALIGN_BASELINE | NVG_ALIGN_LEFT);
    nvgText (ctx->ctx, ctx->last_pos.x, ctx->last_pos.y, text.data(), nullptr);
    nvgRestore (ctx->ctx);
    return true;
}

void Context::draw_image (Image i, Transform matrix) {
    if (i.format() != PixelFormat::ARGB32) {
        return;
    }

    auto image_hash = ctx->image_hash (i);
    auto handle     = ctx->find_image (image_hash);

    if (handle <= 0) {
        size_t length  = i.width() * i.height() * 4;
        auto swap_data = [&]() {
            if (auto d = i.data())
                for (size_t j = 0; j < length; j += 4)
                    std::swap (d[j], d[j + 2]);
        };

        swap_data();

        handle = nvgCreateImageRGBA (ctx->ctx,
                                     i.width(),
                                     i.height(),
                                     NVG_IMAGE_PREMULTIPLIED | NVG_IMAGE_NODELETE,
                                     i.data());

        if (handle <= 0) {
            swap_data();
            return;
        }

        swap_data();
        ctx->add_image (image_hash, handle);
    }

    ScopedSave save (*this);
    int width, height;
    nvgImageSize (ctx->ctx, handle, &width, &height);

    transform (matrix);
    NVGpaint imgPaint = nvgImagePattern (ctx->ctx, 0, 0, width, height, 0, handle, 1.f);
    nvgBeginPath (ctx->ctx);
    nvgRect (ctx->ctx, 0, 0, width, height);
    nvgFillPaint (ctx->ctx, imgPaint);
    fill();
}

} // namespace nvg
} // namespace lvtk
