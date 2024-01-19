// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

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

} // namespace detail

namespace convert {
static int alignment (Alignment align) {
    uint32_t flags = 0;

    if ((align.flags() & Align::LEFT) != 0)
        flags |= NVG_ALIGN_LEFT;
    if ((align.flags() & Align::RIGHT) != 0)
        flags |= NVG_ALIGN_RIGHT;
    if ((align.flags() & Align::CENTER) != 0)
        flags |= NVG_ALIGN_CENTER;
    if ((align.flags() & Align::TOP) != 0)
        flags |= NVG_ALIGN_TOP;
    if ((align.flags() & Align::BOTTOM) != 0)
        flags |= NVG_ALIGN_BOTTOM;
    if ((align.flags() & Align::MIDDLE) != 0)
        flags |= NVG_ALIGN_MIDDLE;

    return static_cast<int> (flags);
};

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
        nvgSave (ctx);
    }

    void restore() {
        nvgRestore (ctx);
        if (! stack.empty()) {
            state = stack.back();
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

private:
    friend class nvg::Context;
    NVGcontext* ctx { nullptr };
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
    set_fill (Color (0.f, 0.f, 0.f, 1.f));
}

Context::~Context() {
    ctx.reset();
}

float Context::scale_factor() const noexcept {
    return ctx->internal_scale;
}

void Context::translate (const Point<int>& pt) {
    nvgTranslate (ctx->ctx, (float) pt.x, (float) pt.y);
}

void Context::transform (const Transform& mat) {
    nvgTransform (ctx->ctx, mat.m00, mat.m01, mat.m02, mat.m10, mat.m11, mat.m12);
}

void Context::begin_path() { nvgBeginPath (ctx->ctx); }
void Context::move_to (float x1, float y1) { nvgMoveTo (ctx->ctx, x1, y1); }
void Context::line_to (float x1, float y1) { nvgLineTo (ctx->ctx, x1, y1); }
void Context::quad_to (float x1, float y1, float x2, float y2) {
    nvgQuadTo (ctx->ctx, x1, y1, x2, y2);
}
void Context::cubic_to (float x1, float y1, float x2, float y2, float x3, float y3) {
    nvgBezierTo (ctx->ctx, x1, y1, x2, y2, x3, y3);
}
void Context::close_path() { nvgClosePath (ctx->ctx); }

void Context::fill() {
    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgFill (ctx->ctx);
}

void Context::stroke() { nvgStroke (ctx->ctx); }

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
}

void Context::set_fill (const Fill& fill) {
    auto c      = fill.color();
    auto& color = ctx->state.color;
    color.r     = c.fred();
    color.g     = c.fgreen();
    color.b     = c.fblue();
    color.a     = c.falpha();
}

void Context::save() { ctx->save(); }
void Context::restore() { ctx->restore(); }

void Context::begin_frame (int width, int height, float scale) {
    ctx->internal_scale = scale;
    ctx->stack.clear();
    ctx->state = {};
    nvgBeginFrame (ctx->ctx,
                   (float) width,
                   (float) height,
                   ctx->internal_scale);
    clip ({ 0, 0, width, height });
}

void Context::end_frame() {
    nvgEndFrame (ctx->ctx);
}

void Context::fill_rect (const Rectangle<float>& r) {
    nvgBeginPath (ctx->ctx);

    nvgRect (ctx->ctx,
             (r.x),
             (r.y),
             r.width,
             r.height);

    fill();
}

bool Context::text (const std::string& text, float x, float y, Alignment align) {
    nvgFontSize (ctx->ctx, ctx->state.font.height());
    nvgFontFaceId (ctx->ctx, ctx->state.font_id);
    nvgTextAlign (ctx->ctx, convert::alignment (align));
    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgText (ctx->ctx, x, y, text.c_str(), nullptr);
    return true;
}

} // namespace nvg
} // namespace lvtk
