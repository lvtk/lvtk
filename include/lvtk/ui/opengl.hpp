// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/view.hpp>
#include <lvtk/ui/widget.hpp>

namespace lvtk {

struct OpenGL : public Backend {
    OpenGL() : Backend ("OpenGL") {}
    std::unique_ptr<View> create_view (Main& c, Widget& w) override;
};

} // namespace lvtk
