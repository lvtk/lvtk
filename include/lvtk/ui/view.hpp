// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/graphics.hpp>
#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Main;
class Widget;

class View {
protected:

public:
    virtual ~View();
    uintptr_t handle();
    double scale() const noexcept;

    void set_visible (bool visible);
    bool visible() const;

    void set_size (int width, int height);
    Bounds bounds() const;

    void realize();
    
protected:
    View (Main& context, Widget& widget);
    void render (Surface& surface);
    void set_backend (uintptr_t);
    virtual void expose (Bounds frame) {}
    virtual void created() {}
    virtual void destroyed() {}

private:
    friend class Main;
    Main& _main;
    Widget& _widget;
    uintptr_t _view;
    WeakRef<Widget> _hovered;

    struct EventHandler;

    void set_parent (uintptr_t parent);

    LVTK_WEAK_REFABLE (View, _weak_status)
};

using ViewRef = WeakRef<View>;

}
