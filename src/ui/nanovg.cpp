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

    if ((align.flags() & Alignment::LEFT) != 0)
        flags |= NVG_ALIGN_LEFT;
    if ((align.flags() & Alignment::RIGHT) != 0)
        flags |= NVG_ALIGN_RIGHT;
    if ((align.flags() & Alignment::CENTER) != 0)
        flags |= NVG_ALIGN_CENTER;
    if ((align.flags() & Alignment::TOP) != 0)
        flags |= NVG_ALIGN_TOP;
    if ((align.flags() & Alignment::BOTTOM) != 0)
        flags |= NVG_ALIGN_BOTTOM;
    if ((align.flags() & Alignment::MIDDLE) != 0)
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
        _font_bold = nvgCreateFontMem (ctx,
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
        if (stack.empty())
            return;
        state = stack.back();
        stack.pop_back();
        nvgRestore (ctx);
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
            color = o.color;
            clip  = o.clip;
            font  = o.font;
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

void Context::clip (const Rectangle<int>& r) {
    ctx->state.clip = r.as<float>();
    nvgScissor (ctx->ctx,
                ctx->state.clip.x,
                ctx->state.clip.y,
                ctx->state.clip.width,
                ctx->state.clip.height);
}

void Context::intersect_clip (const Rectangle<int>& r) {
    nvgIntersectScissor (ctx->ctx, r.x, r.y, r.width, r.height);
}

Rectangle<int> Context::last_clip() const {
    return (ctx->state.clip).as<int>();
}

Font Context::font() const noexcept { return ctx->state.font; }
void Context::set_font (const Font& font) { 
    ctx->state.font = font; 
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
    nvgBeginFrame (ctx->ctx,
                   (float) width,
                   (float) height,
                   ctx->internal_scale);
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

    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgFill (ctx->ctx);
}

bool Context::text (const std::string& text, float x, float y, Alignment align) {
    const auto& font = ctx->state.font;
    nvgFontSize (ctx->ctx, ctx->state.font.height());
    nvgFontFaceId (ctx->ctx, ctx->state.font_id);
    nvgTextAlign (ctx->ctx, convert::alignment (align));
    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgText (ctx->ctx, x, y, text.c_str(), nullptr);
    return true;
}

} // namespace nvg
} // namespace lvtk
