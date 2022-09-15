// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/font.hpp>

#include "gl.hpp"

#include "../nanovg/nanovg.h"
#include "../nanovg/nanovg_gl.h"
#include "Roboto-Regular.ttf.h"
#include "nanovg.hpp"

using Surface = lvtk::nvg::Surface;

namespace lvtk {
namespace nvg {
namespace detail {
static constexpr const char* default_font_face = "sans";

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
    int flags = 0;

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

    return flags;
};

} // namespace convert

class Surface::Context {
    int _font = 0;

public:
    Context() : ctx (detail::create (NVG_ANTIALIAS | NVG_STENCIL_STROKES)) {
        _font = nvgCreateFontMem (ctx, detail::default_font_face, (uint8_t*) Roboto_Regular_ttf, Roboto_Regular_ttf_size, 0);
    }

    ~Context() {
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
    friend class Surface;
    NVGcontext* ctx { nullptr };
    struct State {
        NVGcolor color;
        Rectangle<float> clip;
        Font font;

        State& operator= (const State& o) {
            color = o.color;
            clip  = o.clip;
            font  = o.font;
            return *this;
        }
    };

    float internal_scale = 1.f;
    State state;
    std::vector<State> stack;
};

Surface::Surface()
    : ctx (std::make_unique<Context>()) {
    set_fill (Color (0.f, 0.f, 0.f, 1.f));
}

Surface::~Surface() {
    ctx.reset();
}

float Surface::scale_factor() const noexcept {
    return ctx->internal_scale;
}

void Surface::translate (const Point<int>& pt) {
    nvgTranslate (ctx->ctx, (float) pt.x, (float) pt.y);
}

void Surface::transform (const Transform& mat) {
    nvgTransform (ctx->ctx, mat.m00, mat.m01, mat.m02, mat.m10, mat.m11, mat.m12);
}

void Surface::clip (const Rectangle<int>& r) {
    ctx->state.clip = r.as<float>();
    nvgScissor (ctx->ctx,
                ctx->state.clip.x,
                ctx->state.clip.y,
                ctx->state.clip.width,
                ctx->state.clip.height);
}

void Surface::intersect_clip (const Rectangle<int>& r) {
    nvgIntersectScissor (ctx->ctx, r.x, r.y, r.width, r.height);
}

Rectangle<int> Surface::last_clip() const {
    return (ctx->state.clip).as<int>();
}

Font Surface::font() const noexcept { return ctx->state.font; }
void Surface::set_font (const Font& font) { ctx->state.font = font; }

void Surface::set_fill (const Fill& fill) {
    auto c      = fill.color();
    auto& color = ctx->state.color;
    color.r     = c.fred();
    color.g     = c.fgreen();
    color.b     = c.fblue();
    color.a     = c.falpha();
}

void Surface::save() { ctx->save(); }
void Surface::restore() { ctx->restore(); }

void Surface::begin_frame (int width, int height, float scale) {
    ctx->internal_scale = scale;
    nvgBeginFrame (ctx->ctx,
                   (float) width,
                   (float) height,
                   ctx->internal_scale);
}

void Surface::end_frame() {
    nvgEndFrame (ctx->ctx);
}

void Surface::fill_rect (const Rectangle<float>& r) {
    nvgBeginPath (ctx->ctx);

    nvgRect (ctx->ctx,
             (r.x),
             (r.y),
             r.width,
             r.height);

    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgFill (ctx->ctx);
}

bool Surface::text (const std::string& text, float x, float y, Alignment align) {
    nvgFontSize (ctx->ctx, ctx->state.font.height());
    nvgFontFace (ctx->ctx, detail::default_font_face);
    nvgTextAlign (ctx->ctx, convert::alignment (align));
    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgText (ctx->ctx, x, y, text.c_str(), nullptr);
    return true;
}

} // namespace nvg
} // namespace lvtk
