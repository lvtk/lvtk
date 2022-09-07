#pragma once
#include <lvtk/ui/opengl.hpp>

namespace lvtk {

/** Surface backed by a NanoVG context */
class NanoVGSurface final : public OpenGLSurface {
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
    class Context;
    std::unique_ptr<Context> ctx;
};

/** NanoVG Backend.
    Pass one of these to Main to use NanoVG for rendering
 */
struct NanoVG : OpenGL<NanoVGSurface> {
    NanoVG() : OpenGL ("NanoVG") {}
};

}
