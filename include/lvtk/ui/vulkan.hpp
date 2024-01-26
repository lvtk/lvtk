// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/main.hpp>

namespace lvtk {

class Main;
class Widget;

/** The Vulkan graphics backend.
    Using this backend requires linking with lvtk-vulkan-3.0

    @ingroup widgets
    @ingroup graphics
    @headerfile lvtk/ui/vulkan.hpp 
*/
struct LVTK_API Vulkan : public Backend {
    Vulkan() : Backend ("Vulkan") {}
    std::unique_ptr<View> create_view (Main& c, Widget& w) override;
};

} // namespace lvtk
