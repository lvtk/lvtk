// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/graphics.hpp>
#include <lvtk/ui/style.hpp>
#include <lvtk/weak_ref.hpp>

/** @ingroup widgets 
    @{ 
*/

namespace lvtk {

namespace detail {
    /** @private */
    class View;
}

class Main;
class Widget;

/** Flags to configure a View 
    @ingroup widgets
    @headerfile lvtk/ui/view.hpp 
*/
struct LVTK_API ViewFlag {
    /** Flags */
    enum : uint32_t {
        TRIVIAL   = (1u << 0u), ///< View is trivial, like a popup menu.
        RESIZABLE = (1u << 1u)  ///< View is resizable.
    };
};

/** Flag type for views.
    @ingroup widgets
    @headerfile lvtk/ui/view.hpp 
*/
using ViewFlags = uint32_t;

/** A View.  e.g. OS window that renders a Widget.
    @ingroup widgets
    @headerfile lvtk/ui/view.hpp 
*/
class LVTK_API View {
public:
    virtual ~View();

    /** Returns the native window handle */
    uintptr_t handle();

    /** Returns the scale factor of this view. */
    float scale_factor() const noexcept;

    /** Show or hide this View */
    void set_visible (bool visible);

    /** Returns true if visible */
    bool visible() const;

    /** Resize this View */
    void set_size (int width, int height);

    /** Get the postion and size of this view */
    Bounds bounds() const;

    /** Change the position or size of this view */
    void set_bounds (Bounds b);

    /** Returns the Main that initialized this view */
    Main& main() noexcept { return _main; }

    /** Returns the Style used by this view */
    Style& style() noexcept;
    const Style& style() const noexcept;

    /** Elevate a widget to view Status using this one as the Parent
        i.e. Show it in an OS window.

        @param widget The Widget to eleveate.
        @param flags  Flags to use on the new child view.
    */
    void elevate (Widget& widget, ViewFlags flags);

    /** Repaint a region of the view */
    void repaint (Bounds bounds);

protected:
    View (Main& context, Widget& widget);
    void render (DrawingContext& surface);
    void set_backend (uintptr_t);
    void set_view_hint (int, int);

    /** Override this to render the widget */
    virtual void expose (Bounds frame) {}
    /** Called when the view is first created */
    virtual void created() {}
    /** Called when the view has been destroyed */
    virtual void destroyed() {}

private:
    friend class detail::View;
    friend class Main;
    
    Main& _main;
    Widget& _widget;
    uintptr_t _view;
    WeakRef<Widget> _hovered;

    void set_parent (uintptr_t parent, bool transient);
    void realize();

    struct EventHandler;
    std::unique_ptr<detail::View> impl;
    LVTK_WEAK_REFABLE (View);
};

using ViewRef = WeakRef<View>;

} // namespace lvtk

/** @} */
