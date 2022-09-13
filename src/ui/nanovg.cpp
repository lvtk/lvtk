// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/nanovg.hpp>

#include "Roboto-Regular.ttf.h"
#include "nanovg/nanovg_gl.h"

using Surface = lvtk::nvg::Surface;

namespace lvtk {
namespace nvg {
namespace detail {
static constexpr const char* default_font_face = "sans";

#ifdef NANOVG_GL2
static constexpr auto create = nvgCreateGL2;
static constexpr auto destroy = nvgDeleteGL2;
#else
static constexpr auto create = nvgCreateGL3;
static constexpr auto destroy = nvgDeleteGL3;
#endif
} // namespace detail

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
        Point<float> origin;
        Rectangle<float> clip;

        State& operator= (const State& o) {
            color = o.color;
            origin = o.origin;
            clip = o.clip;
            return *this;
        }
    };

    float scale = 1.f;
    State state;
    std::vector<State> stack;
};

Surface::Surface (float scale)
    : ctx (std::make_unique<Context>()) {
    ctx->scale = scale;
    set_fill (Color (0.f, 0.f, 0.f, 1.f));
}

Surface::~Surface() {
    ctx.reset();
}

float Surface::scale_factor() const noexcept {
    return ctx->scale;
}

void Surface::translate (const Point<int>& pt) {
    auto& state = ctx->state;
    state.origin += pt.as<float>() * ctx->scale;
}

void Surface::set_clip_bounds (const Rectangle<int>& r) {
    auto rf = r.as<float>() * ctx->scale;
    if (rf == ctx->state.clip)
        return;
    nvgScissor (ctx->ctx, rf.x, rf.y, rf.width, rf.height);
    ctx->state.clip = rf;
}

Rectangle<int> Surface::clip_bounds() const {
    return (ctx->state.clip / ctx->scale).as<int>();
}

void Surface::set_fill (const Fill& fill) {
    auto c = fill.color();
    auto& color = ctx->state.color;
    color.r = c.fred();
    color.g = c.fgreen();
    color.b = c.fblue();
    color.a = c.falpha();
}

void Surface::save() { ctx->save(); }
void Surface::restore() { ctx->restore(); }

void Surface::begin_frame (int width, int height, float scale) {
    // are nvg pixel ratio and PuglView scale same?
    auto pixel_ratio = ctx->scale = scale;
    nvgBeginFrame (ctx->ctx, width, height, pixel_ratio);
}

void Surface::end_frame() {
    nvgEndFrame (ctx->ctx);
}

void Surface::fill_rect (const Rectangle<float>& r) {
    nvgBeginPath (ctx->ctx);

    nvgRect (ctx->ctx,
             (r.x * ctx->scale) + ctx->state.origin.x,
             (r.y * ctx->scale) + ctx->state.origin.y,
             r.width * ctx->scale,
             r.height * ctx->scale);

    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgFill (ctx->ctx);
}

void Surface::__text_top_left (const std::string& text, float x, float y) {
    nvgFontSize (ctx->ctx, 15.f * ctx->scale);
    nvgFontFace (ctx->ctx, detail::default_font_face);
    nvgTextAlign (ctx->ctx, NVG_ALIGN_TOP | NVG_ALIGN_LEFT);
    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgText (ctx->ctx,
             ctx->state.origin.x + (x * ctx->scale),
             ctx->state.origin.y + (y * ctx->scale),
             text.c_str(),
             nullptr);
}

} // namespace nvg
} // namespace lvtk
