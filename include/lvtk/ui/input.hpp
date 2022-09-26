// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/graphics.hpp> // point
#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Main;
class View;
class Widget;

/** An event type sent about user input.
    @ingroup widgets
*/
struct Event {
    Main& main;             ///< Context of the event.
    WeakRef<View> view;     ///< Originating view.
    WeakRef<Widget> source; ///< The source Widget.
    WeakRef<Widget> target; ///< The source Widget.
    Point<float> pos;       ///< High resolution position.
    int x;                  ///< Rounded X
    int y;                  ///< Rounded Y

    int num_clicks; ///< Number of clicks

    Event (Main& m) : main (m) {}
    Event (Main& m, View& v, Widget& s)
        : main (m), view (&v), source (&s) {}
    Event (Main& m, View& v, Widget& s, Widget& t)
        : main (m), view (&v), source (&s), target (&t) {}
};

} // namespace lvtk
