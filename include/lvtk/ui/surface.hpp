// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/fill.hpp>
#include <lvtk/ui/path.hpp>
#include <lvtk/ui/rectangle.hpp>

namespace lvtk {

class Surface {
public:
    Surface() = default;
    virtual ~Surface() = default;

    virtual void translate (const Point<int>& pt) = 0;

    virtual void set_clip_bounds (const Rectangle<int>& r) = 0;
    virtual Rectangle<int> clip_bounds() const = 0;

    virtual void save() = 0;
    virtual void restore() = 0;

    virtual void set_fill (const Fill& fill) = 0;
    virtual void fill_rect (const Rectangle<float>& r) = 0;

    virtual void __text (const std::string& text, float x, float y) {}
};

} // namespace lvtk
