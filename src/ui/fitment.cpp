// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/fitment.hpp>

namespace lvtk {

bool Fitment::operator== (const Fitment& o) const noexcept {
    return _flags == o._flags;
}

bool Fitment::operator!= (const Fitment& o) const noexcept {
    return _flags != o._flags;
}

void Fitment::apply (double& x, double& y, double& w, double& h,
                     const double dx, const double dy,
                     const double dw, const double dh) const noexcept {
    if (w == 0.0 || h == 0.0)
        return;

    if ((_flags & STRETCH) != 0) {
        x = dx;
        y = dy;
        w = dw;
        h = dh;
    } else {
        double scale = (_flags & FILL) != 0 ? std::max (dw / w, dh / h)
                                            : std::min (dw / w, dh / h);

        if ((_flags & NO_GROW) != 0)
            scale = std::min (scale, 1.0);

        if ((_flags & NO_SHRINK) != 0)
            scale = std::max (scale, 1.0);

        w *= scale;
        h *= scale;

        if ((_flags & LEFT) != 0)
            x = dx;
        else if ((_flags & RIGHT) != 0)
            x = dx + dw - w;
        else
            x = dx + (dw - w) * 0.5;

        if ((_flags & TOP) != 0)
            y = dy;
        else if ((_flags & BOTTOM) != 0)
            y = dy + dh - h;
        else
            y = dy + (dh - h) * 0.5;
    }
}

Transform Fitment::transform (const Rectangle<double>& src, const Rectangle<double>& dst) const noexcept {
    if (src.empty())
        return {};

    auto new_x = dst.x;
    auto new_y = dst.y;

    auto scale_x = dst.width / src.width;
    auto scale_y = dst.height / src.height;

    if ((_flags & STRETCH) == 0) {
        scale_x = (_flags & FILL) != 0 ? std::max (scale_x, scale_y)
                                       : std::min (scale_x, scale_y);

        if ((_flags & NO_GROW) != 0)
            scale_x = std::min (scale_x, 1.0);

        if ((_flags & NO_SHRINK) != 0)
            scale_x = std::max (scale_x, 1.0);

        scale_y = scale_x;

        if ((_flags & RIGHT) != 0)
            new_x += dst.width - src.width * scale_x; // right
        else if ((_flags & LEFT) == 0)
            new_x += (dst.width - src.width * scale_x) / 2.0; // centerd

        if ((_flags & BOTTOM) != 0)
            new_y += dst.height - src.height * scale_x; // bottom
        else if ((_flags & TOP) == 0)
            new_y += (dst.height - src.height * scale_x) / 2.0; // centerd
    }

    return Transform::translation (-src.x, -src.y)
        .scaled (scale_x, scale_y)
        .translated (new_x, new_y);
}

} // namespace lvtk
