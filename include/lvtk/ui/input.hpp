// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/graphics.hpp> // point
#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Widget;

/** @ingroup widgets */
struct InputEvent {
    WeakRef<Widget> source;
    Point<float> pos;
    int x;
    int y;
};

/** @ingroup widgets */
struct Event : public InputEvent {};

} // namespace lvtk
