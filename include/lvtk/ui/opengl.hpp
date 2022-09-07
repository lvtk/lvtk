// Copyright 2022 Michael Fisher <mfisher@kushview.net>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/surface.hpp>
#include <lvtk/ui/view.hpp>

#define PUGL_DISABLE_DEPRECATED
#include <pugl/gl.h>

namespace lvtk {

/** Surface which uses OpenGL directly. */
class OpenGLSurface : public Surface {
public:
    OpenGLSurface() = default;
    virtual ~OpenGLSurface() = default;

// private:
//     friend class OpenGLView<OpenGLSurface>;
    /** called immediately before rendering */
    virtual void begin_frame (int width, int height, float scale) {}
    /** called immediately after rendering */
    virtual void end_frame() {}
};

/** Surf template param must be an OpenGLView of some kind */
template <class Surf>
class OpenGLView : public View {
public:
    using surface_type = Surf;

    /** OpenGL base view.
        @param context The context creating the fiew
        @param widget  The widget being elevated that will own this view;
     */
    OpenGLView (Main& context, Widget& widget)
        : View (context, widget) {
        set_backend ((uintptr_t) puglGlBackend());
    }

    ~OpenGLView() {
        _surface.reset();
    }

protected:
    void created() override {
        View::created();
        if (! _surface) {
            _surface = std::make_unique<Surf>();
        }
    }

    void expose (Bounds frame) {
        glClearColor (0.f, 0.f, 0.f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _surface->begin_frame (frame.width, frame.height, scale());
        render (*_surface);
        _surface->end_frame();
    }

private:
    std::unique_ptr<OpenGLSurface> _surface;
};

template<class Surf, class V = OpenGLView<Surf>>
struct OpenGL : public Backend {
    using surface_type = Surf;
    using view_type = V;
    OpenGL (const std::string& name) : Backend (name) {}
    std::unique_ptr<View> create_view (Main& c, Widget& w) override {
        return std::make_unique<V> (c, w);
    }
};

} // namespace lvtk
