// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/graphics.hpp>

namespace lvtk {
namespace nvg {

/** DrawingContext backed by a NanoVG context */
class Context final : public lvtk::DrawingContext {
public:
    Context();
    ~Context();

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
    class Ctx;
    std::unique_ptr<Ctx> ctx;
};

} // namespace nvg
} // namespace lvtk
