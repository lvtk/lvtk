// Copyright 2022 Michael Fisher <mfisher@kushview.net>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/graphics.hpp>
#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Main;
class Widget;

class View {
protected:
    View (Main& context, Widget& widget);

public:
    virtual ~View();
    uintptr_t handle();
    void set_parent (uintptr_t parent);
    double scale() const noexcept;

    void set_visible (bool visible);
    bool visible() const;

    void set_size (int width, int height);
    Bounds bounds() const;

    void realize();
    
protected:
    void render (Surface& surface);
    void set_backend (uintptr_t);
    virtual void expose (Bounds frame) {}
    virtual void created() {}
    virtual void destroyed() {}

private:
    void configure (Bounds);
    void pointer_in() {}
    void pointer_out() {}

    friend class Main;
    Main& _main;
    Widget& _widget;
    uintptr_t _view;
    WeakRef<Widget> _hovered;

    struct EventHandler;
    LVTK_WEAK_REFABLE (View, _weak_status)
    struct Impl;
};

using ViewRef = WeakRef<View>;

}
