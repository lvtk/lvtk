// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Widget;

struct InputEvent {
    WeakRef<Widget> source;
    Point<double> pos;
    int x;
    int y;
};

// saving for later...
struct Event : public InputEvent {};

} // namespace lvtk
