// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/surface.hpp>
#include <lvtk/ui/view.hpp>

#ifdef GL_SILENCE_DEPRECATION
#    undef GL_SILENCE_DEPRECATION
#endif
#define GL_SILENCE_DEPRECATION
#define PUGL_DISABLE_DEPRECATED
#include <pugl/gl.h>
#include <pugl/pugl.h>

// /// A hint for configuring a view
// typedef enum {
//   PUGL_USE_COMPAT_PROFILE,    ///< Use compatible (not core) OpenGL profile
//   PUGL_USE_DEBUG_CONTEXT,     ///< True to use a debug OpenGL context
//   PUGL_CONTEXT_VERSION_MAJOR, ///< OpenGL context major version
//   PUGL_CONTEXT_VERSION_MINOR, ///< OpenGL context minor version
//   PUGL_RED_BITS,              ///< Number of bits for red channel
//   PUGL_GREEN_BITS,            ///< Number of bits for green channel
//   PUGL_BLUE_BITS,             ///< Number of bits for blue channel
//   PUGL_ALPHA_BITS,            ///< Number of bits for alpha channel
//   PUGL_DEPTH_BITS,            ///< Number of bits for depth buffer
//   PUGL_STENCIL_BITS,          ///< Number of bits for stencil buffer
//   PUGL_SAMPLES,               ///< Number of samples per pixel (AA)
//   PUGL_DOUBLE_BUFFER,         ///< True if double buffering should be used
//   PUGL_SWAP_INTERVAL,         ///< Number of frames between buffer swaps
//   PUGL_RESIZABLE,             ///< True if view should be resizable
//   PUGL_IGNORE_KEY_REPEAT,     ///< True if key repeat events are ignored
//   PUGL_REFRESH_RATE,          ///< Refresh rate in Hz
// } PuglViewHint;

namespace lvtk {

/** Surface which uses OpenGL directly. */
class OpenGLSurface : public Surface {
public:
    OpenGLSurface() = default;
    virtual ~OpenGLSurface() = default;

    /** Called immediately before rendering
        
        @param width Width in logical coords
        @param height Height in logical coords
        @param scale Scale factor in use by @ref OpenGLView
    */
    virtual void begin_frame (int width, int height, float scale) =0;

    /** called immediately after rendering */
    virtual void end_frame() =0;
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
        set_view_hint (PUGL_CONTEXT_VERSION_MAJOR, 3);
        set_view_hint (PUGL_CONTEXT_VERSION_MINOR, 3);
        set_view_hint (PUGL_DOUBLE_BUFFER, PUGL_TRUE);
        set_view_hint (PUGL_RESIZABLE, PUGL_TRUE);
    }

    ~OpenGLView() {
        _surface.reset();
    }

protected:
    inline void created() override {
        if (! _surface) {
            _surface = std::make_unique<Surf>();
        }
        View::created();
    }

    inline void expose (Bounds frame) override {
        auto vframe = bounds() * scale_factor();
        auto gl_frame = frame * scale_factor();
        // flip y-coord
        gl_frame.y = vframe.height - gl_frame.y - gl_frame.height;
        glViewport (gl_frame.x, gl_frame.y, gl_frame.width, gl_frame.height);
        glClearColor (0.f, 0.f, 0.f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (_surface) {
            _surface->begin_frame (frame.width, frame.height, scale_factor());
            render (*_surface);
            _surface->end_frame();
        }
    }

private:
    std::unique_ptr<OpenGLSurface> _surface;
};

template <class Surf, class V = OpenGLView<Surf>>
struct OpenGL : public Backend {
    using surface_type = Surf;
    using view_type = V;
    OpenGL (const std::string& name) : Backend (name) {}
    std::unique_ptr<View> create_view (Main& c, Widget& w) override {
        return std::make_unique<V> (c, w);
    }
};

} // namespace lvtk
