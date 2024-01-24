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

    void begin_frame (int width, int height, double pixel_ratio);
    void end_frame();

    double scale_factor() const noexcept override;
    void save() override;
    void restore() override;

    void set_line_width (double width) override;

    void clear_path() override;
    void move_to (double x1, double y1) override;
    void line_to (double x1, double y1) override;
    void quad_to (double x1, double y1, double x2, double y2) override;
    void cubic_to (double x1, double y1, double x2, double y2, double x3, double y3) override;
    void close_path() override;

    void fill() override;
    void stroke() override;

    void translate (double, double) override;
    void transform (const Transform& mat) override;
    void clip (const Rectangle<int>& r) override;
    void exclude_clip (const Rectangle<int>& r) override;
    Rectangle<int> last_clip() const override;
    void set_fill (const Fill& fill) override;

    void fill_rect (const Rectangle<double>& r) override;

    FontMetrics font_metrics() const noexcept override;
    TextMetrics text_metrics (std::string_view text) const noexcept override;
    bool show_text (std::string_view) override;
    void draw_image (Image i, Transform matrix) override;

    Font font() const noexcept override;
    void set_font (const Font& font) override;

private:
    class Ctx;
    std::unique_ptr<Ctx> ctx;
};

} // namespace nvg
} // namespace lvtk
