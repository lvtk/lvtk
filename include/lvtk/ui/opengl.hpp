// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/view.hpp>
#include <lvtk/ui/widget.hpp>

namespace lvtk {

/** The OpenGL graphics backend.
    Using this backend requires linking with lvtk-opengl-3.0

    @ingroup widgets
    @ingroup graphics
    @headerfile lvtk/ui/opengl.hpp 
*/
struct LVTK_API OpenGL : public Backend {
    OpenGL() : Backend ("OpenGL") {}
    std::unique_ptr<View> create_view (Main& c, Widget& w) override;
};

} // namespace lvtk
