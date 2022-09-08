// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once
#include <lvtk/ui/opengl.hpp>

namespace lvtk {
namespace nvg {

/** Surface backed by a NanoVG context */
class Surface final : public OpenGLSurface {
public:
    Surface();
    ~Surface();

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
    class Context;
    std::unique_ptr<Context> ctx;
};

inline static constexpr const char* GL_BACKEND_NAME = "NanoVG";

}

/** NanoVG Backend.
    Pass one of these to Main to use NanoVG for rendering
 */
struct NanoVG : OpenGL<nvg::Surface> {
    NanoVG() : OpenGL (nvg::GL_BACKEND_NAME) {}
};

}
