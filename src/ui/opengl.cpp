
#include "lvtk/ui/opengl.hpp"
#include "lvtk/ui/graphics.hpp"
#include "lvtk/ui/main.hpp"

#define PUGL_DISABLE_DEPRECATED
#include <pugl/gl.h>
#define NANOVG_GL3 1
#include "nanovg/nanovg_gl.h"

namespace lvtk {

class NanoVGSurface final : public Surface {
public:
    NanoVGSurface();
    ~NanoVGSurface();

    void translate (const Point<int>& pt) override;
    void set_clip_bounds (const Rectangle<int>& r) override;
    Rectangle<int> clip_bounds() const override;
    void set_fill (const Fill& fill) override;
    void save() override;
    void restore() override;
    void begin_frame (int width, int height, float pixel_ratio);
    void end_frame();
    void fill_rect (const Rectangle<float>& r) override;

private:
    class Ctx;
    std::unique_ptr<Ctx> ctx;
};

class NanoVGSurface::Ctx {
public:
    Ctx() : ctx (nvgCreateGL3 (NVG_ANTIALIAS | NVG_STENCIL_STROKES)) {}
    ~Ctx() { nvgDeleteGL3 (ctx); }

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
    friend class NanoVGSurface;
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

NanoVGSurface::NanoVGSurface()
    : ctx (std::make_unique<Ctx>()) {
    set_fill (Color (0.f, 0.f, 0.f, 1.f));
}

NanoVGSurface::~NanoVGSurface() {
    ctx.reset();
}

void NanoVGSurface::translate (const Point<int>& pt) {
    auto& state = ctx->state;
    state.origin += pt.as<float>();
}

void NanoVGSurface::set_clip_bounds (const Rectangle<int>& r) {
    auto rf = r.as<float>();
    if (rf == ctx->state.clip)
        return;
    nvgScissor (ctx->ctx, rf.x, rf.y, rf.width, rf.height);
    ctx->state.clip = rf;
}

Rectangle<int> NanoVGSurface::clip_bounds() const {
    return ctx->state.clip.as<int>();
}

void NanoVGSurface::set_fill (const Fill& fill) {
    auto c = fill.color();
    auto& state = ctx->state;
    state.color.r = c.fred();
    state.color.g = c.fgreen();
    state.color.b = c.fblue();
    state.color.a = c.falpha();
}

void NanoVGSurface::save() { ctx->save(); }
void NanoVGSurface::restore() { ctx->restore(); }

void NanoVGSurface::begin_frame (int width, int height, float pixel_ratio) {
    nvgBeginFrame (ctx->ctx, width, height, pixel_ratio);
}

void NanoVGSurface::end_frame() {
    nvgEndFrame (ctx->ctx);
}

void NanoVGSurface::fill_rect (const Rectangle<float>& r) {
    nvgBeginPath (ctx->ctx);

    nvgRect (ctx->ctx,
             r.x + ctx->state.origin.x,
             r.y + ctx->state.origin.y,
             r.width,
             r.height);

    nvgFillColor (ctx->ctx, ctx->state.color);
    nvgFill (ctx->ctx);
}

//=============================================================================
class OpenGLView : public View {
public:
    OpenGLView (Main& c, Widget& w)
        : View (c, w) {
        set_backend ((uintptr_t) puglGlBackend());
    }

protected:
    void created() override {
        View::created();
        if (! surface) {
            surface = std::make_unique<NanoVGSurface>();
        }
    }

    void expose (Rectangle<int> frame) override {
        glClearColor (0.f, 0.f, 0.f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        surface->begin_frame (frame.width, frame.height, scale());
        render (*surface);
        surface->end_frame();
    }

private:
    std::unique_ptr<NanoVGSurface> surface;
};

//=============================================================================
std::unique_ptr<View> OpenGL::create_view (Main& m, Widget& w) {
    return std::make_unique<OpenGLView> (m, w);
}

} // namespace lvtk
