// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/main.hpp>

namespace lvtk {

class Main;
class Widget;

/** The Vulkan graphics backend.
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
