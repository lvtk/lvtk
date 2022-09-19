// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/graphics.hpp>
#include <lvtk/ui/style.hpp>
#include <lvtk/weak_ref.hpp>

namespace lvtk {

class Main;
class Widget;

struct LVTK_API ViewFlag {
    enum : uint32_t {
        TRIVIAL   = (1u << 0u), // View is trivial, like a popup menu.
        RESIZABLE = (1u << 1u)  // View is resizable.
    };
};

using ViewFlags = uint32_t;

class LVTK_API View {
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

    Main& main() noexcept { return _main; }
    Style& style() noexcept;
    const Style& style() const noexcept;

    void elevate (Widget& widget, ViewFlags flags);

    void repaint (Bounds);

protected:
    View (Main& context, Widget& widget);
    void render (DrawingContext& surface);
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

    void set_parent (uintptr_t parent, bool transient);
    void realize();

    struct EventHandler;
    LVTK_WEAK_REFABLE (View);
};

using ViewRef = WeakRef<View>;

} // namespace lvtk
