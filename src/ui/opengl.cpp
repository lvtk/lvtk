// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <cassert>

#include "gl.hpp"
#include "nanovg.hpp"
#include <lvtk/ui/opengl.hpp>

namespace lvtk {

using OpenGLContext = nvg::Context;

/** Surf template param must be an OpenGLView of some kind */
template <class Surf>
class OpenGLView : public lvtk::View {
public:
    using surface_type = Surf;

    /** OpenGL base view.
        @param context The context creating the fiew
        @param widget  The widget being elevated that will own this view;
     */
    OpenGLView (Main& context, Widget& widget)
        : lvtk::View (context, widget) {
        set_backend ((uintptr_t) puglGlBackend());
        set_view_hint (PUGL_CONTEXT_VERSION_MAJOR, 3);
        set_view_hint (PUGL_CONTEXT_VERSION_MINOR, 3);
        set_view_hint (PUGL_DOUBLE_BUFFER, PUGL_TRUE);
    }

    ~OpenGLView() {
        _surface.reset();
    }

protected:
    inline void created() override {
        if (! _surface) {
#if _WIN32
            static bool glad_loaded = false;
            if (! glad_loaded) {
                glad_loaded = gladLoadGL() != 0;
                assert(glad_loaded);
            }
#endif
            _surface = std::make_unique<Surf>();
        }
        View::created();
    }

    inline void expose (Bounds frame) override {
        const auto scale = scale_factor();
        const auto vb    = bounds().at (0);
        glViewport (0, 0, vb.width * scale, vb.height * scale);

        if (needs_cleared) {
            glClearColor (bg_color.fred(), bg_color.fgreen(), bg_color.fblue(), bg_color.falpha());
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            needs_cleared = false;
        }

        if (_surface) {
            _surface->begin_frame (vb.width, vb.height, scale);
            _surface->save();
            _surface->clip (frame);
            render (*_surface);
            _surface->restore();
            _surface->end_frame();
        }
    }

private:
    std::unique_ptr<Surf> _surface;
    bool needs_cleared = true;
    Color bg_color { 0x000000ff };
};

std::unique_ptr<View> OpenGL::create_view (Main& c, Widget& w) {
    return std::make_unique<OpenGLView<OpenGLContext>> (c, w);
}

} // namespace lvtk
