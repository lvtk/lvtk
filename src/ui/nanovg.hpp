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
    void save() override;
    void restore() override;

    void begin_path() override;
    void move_to (float x1, float y1) override;
    void line_to (float x1, float y1) override;
    void quad_to (float x1, float y1, float x2, float y2) override;
    void cubic_to (float x1, float y1, float x2, float y2, float x3, float y3) override;
    void close_path() override;

    void fill() override;
    void stroke() override;

    void translate (const Point<int>& pt) override;
    void transform (const Transform& mat) override;
    void clip (const Rectangle<int>& r) override;
    void exclude_clip (const Rectangle<int>& r) override;
    Rectangle<int> last_clip() const override;
    void set_fill (const Fill& fill) override;

    void fill_rect (const Rectangle<float>& r) override;

    FontMetrics font_metrics() const noexcept override;
    TextMetrics text_metrics (std::string_view text) const noexcept override;
    bool show_text (std::string_view) override;

    Font font() const noexcept override;
    void set_font (const Font& font) override;

private:
    class Ctx;
    std::unique_ptr<Ctx> ctx;
};

} // namespace nvg
} // namespace lvtk
