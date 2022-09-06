// Copyright 2022 Michael Fisher <mfisher@kushview.net>
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

}
