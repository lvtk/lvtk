// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/nanovg.hpp>

#include "nanovg/nanovg_gl.h"
// #include "Roboto-Regular.ttf.h"

using Surface = lvtk::nvg::Surface;

namespace lvtk {
namespace nvg {

#ifdef NANOVG_GL2
static constexpr auto create = nvgCreateGL2;
static constexpr auto destroy = nvgDeleteGL2;
#else
static constexpr auto create = nvgCreateGL3;
static constexpr auto destroy = nvgDeleteGL3;
#endif

class Surface::Context {
    int _font = 0;
public:
    Context() : ctx (nvg::create (NVG_ANTIALIAS | NVG_STENCIL_STROKES)) 
    {
        // nvgCreateFontMem (ctx, "sans", 
        //                  (uint8_t*)Roboto_Regular_ttf,
        //                   Roboto_Regular_ttf_size, 1);
    }
    
    ~Context() {
        if (ctx)
            nvg::destroy (ctx);
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

void Surface::translate (const Point<int>& pt) {
    auto& state = ctx->state;
    state.origin += pt.as<float>();
}

void Surface::set_clip_bounds (const Rectangle<int>& r) {
    auto rf = r.as<float>();
    if (rf == ctx->state.clip)
        return;
    nvgScissor (ctx->ctx, rf.x, rf.y, rf.width, rf.height);
    ctx->state.clip = rf;
}

Rectangle<int> Surface::clip_bounds() const {
    return ctx->state.clip.as<int>();
}

void Surface::set_fill (const Fill& fill) {
    auto c = fill.color();
    auto& state = ctx->state;
    state.color.r = c.fred();
    state.color.g = c.fgreen();
    state.color.b = c.fblue();
    state.color.a = c.falpha();
}

void Surface::save() { ctx->save(); }
void Surface::restore() { ctx->restore(); }

void Surface::begin_frame (int width, int height, float scale) {
    // are nvg pixel ratio and PuglView scale same?
    auto pixel_ratio = scale;
    nvgBeginFrame (ctx->ctx, width, height, pixel_ratio);
}

void Surface::end_frame() {
    nvgEndFrame (ctx->ctx);
}

void Surface::fill_rect (const Rectangle<float>& r) {
    nvgBeginPath (ctx->ctx);

    nvgRect (ctx->ctx,
             r.x + ctx->state.origin.x,
             r.y + ctx->state.origin.y,
             r.width,
             r.height);

    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgFill (ctx->ctx);
}

} // namespace nvg
} // namespace lvtk
