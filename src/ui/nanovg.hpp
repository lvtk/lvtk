// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/rectangle.hpp>
#include <lvtk/ui/surface.hpp>

namespace lvtk {
namespace nvg {

/** Surface backed by a NanoVG context */
class Surface final : public lvtk::Surface {
public:
    Surface();
    ~Surface();

    void begin_frame (int width, int height, float pixel_ratio);
    void end_frame();

    float scale_factor() const noexcept override;
    void translate (const Point<int>& pt) override;
    void transform (const Transform& mat) override;
    void clip (const Rectangle<int>& r) override;
    void intersect_clip (const Rectangle<int>& r) override;
    Rectangle<int> last_clip() const override;
    void set_fill (const Fill& fill) override;
    void save() override;
    void restore() override;
    void fill_rect (const Rectangle<float>& r) override;
    bool text (const std::string&, float, float, Alignment) override;

    Font font() const noexcept override;
    void set_font (const Font& font) override;

private:
    class Context;
    std::unique_ptr<Context> ctx;
};

inline static constexpr const char* GL_BACKEND_NAME = "NanoVG";

} // namespace nvg
} // namespace lvtk
