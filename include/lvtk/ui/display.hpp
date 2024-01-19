// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.hpp>
#include <lvtk/ui/rectangle.hpp>

namespace lvtk {

struct LVTK_API Display {
    bool primary { false };
    double scale { 1.0 };
    double dpi { 96.0 };
};

} // namespace lvtk
