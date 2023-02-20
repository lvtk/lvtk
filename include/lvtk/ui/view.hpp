// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <boost/signals2/signal.hpp>

#include <lvtk/lvtk.h>
#include <lvtk/ui/graphics.hpp>
#include <lvtk/ui/style.hpp>
#include <lvtk/weak_ref.hpp>

/** @ingroup widgets 
    @{ 
*/

namespace lvtk {
namespace detail {
class Main;
class View;
class Widget;
} // namespace detail

class Main;
class Widget;

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
    /** Flags */
    enum Flag : ViewFlags {
        TRIVIAL   = (1u << 0u), ///< View is trivial, like a popup menu.
        RESIZABLE = (1u << 1u)  ///< View is resizable.
    };

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
    Main& main() noexcept;

    /** Returns the Style used by this view */
    Style& style() noexcept;

    /** Returns the Style used by this view */
    const Style& style() const noexcept;

    /** Returns the Widget that owns this View */
    Widget& widget() noexcept;

    /** Returns the Widget that owns this View */
    const Widget& widget() const noexcept;

    /** Elevate a widget to view Status using this one as the Parent
        i.e. Show it in an OS window.

        @param widget The Widget to eleveate.
        @param flags  Flags to use on the new child view.
     */
    void elevate (Widget& widget, ViewFlags flags);

    /** Repaint a region of the view */
    void repaint (Bounds bounds);

    using IdleSlot = boost::signals2::signal<void()>::slot_type;
    boost::signals2::connection connect_idle (const IdleSlot& slot);

protected:
    View (Main& context, Widget& widget);

    /** Subclasses should use this to render it's context */
    void render (DrawingContext& surface);

    /** Subclasses should use this to set a PuglBackend */
    void set_backend (uintptr_t);

    /** Subclasses should use this to set a PuglViewHint */
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
    friend class detail::Main;
    friend class Widget;
    friend class detail::Widget;
    std::unique_ptr<detail::View> impl;
    LVTK_WEAK_REFABLE (View);
};

using ViewRef = WeakRef<View>;

} // namespace lvtk

/** @} */
