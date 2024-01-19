// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/view.hpp>
#include <lvtk/ui/widget.hpp>

namespace lvtk {

/** The Cairo graphics backend.
    Using this backend requires linking with lvtk-cairo-3.0

    @ingroup widgets
    @ingroup graphics
    @headerfile lvtk/ui/cairo.hpp 
*/
struct LVTK_API Cairo : public Backend {
    Cairo() : Backend ("Cairo") {}
    std::unique_ptr<View> create_view (Main& c, Widget& w) override;
};

} // namespace lvtk
