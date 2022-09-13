// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/graphics.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/style.hpp>
#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Widget;

class View {
protected:
public:
    virtual ~View();
    uintptr_t handle();

    /** Returns the scale factor of this view. */
    float scale_factor() const noexcept;

    void set_visible (bool visible);
    bool visible() const;

    void set_size (int width, int height);
    Bounds bounds() const;
    void set_bounds (Bounds b);

    /** Add this view to the desktop. */
    void realize();

    Style& style() noexcept { return _main.style(); }
    const Style& style() const noexcept { return _main.style(); }

    //====== TESTING ========
    void __pugl_post_redisplay();
    //=======================
protected:
    View (Main& context, Widget& widget);
    void render (Surface& surface);
    void set_backend (uintptr_t);
    void set_view_hint (int, int);

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

    LVTK_WEAK_REFABLE (View);
};

using ViewRef = WeakRef<View>;

} // namespace lvtk
