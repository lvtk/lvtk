// Copyright 2022 Michael Fisher <mfisher@kushview.net>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ui/main.hpp"

#define PUGL_DISABLE_DEPRECATED
#include <pugl/gl.h>

namespace lvtk {

struct OpenGL final : public Backend {
    OpenGL() : Backend ("OpenGL") {}
    std::unique_ptr<View> create_view (Main& c, Widget& w) override;
};

}
