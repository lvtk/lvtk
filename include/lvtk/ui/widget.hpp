// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

/** @defgroup widgets Widgets
    Part of the lvtk Widget's library.

    @{
*/

#pragma once

#include <vector>

#include <lv2/ui/ui.h>

#include <lvtk/lvtk.h>
#include <lvtk/ui/graphics.hpp>
#include <lvtk/ui/input.hpp>
#include <lvtk/ui/view.hpp>
#include <lvtk/weak_ref.hpp>

namespace lvtk {
namespace detail {
/** @private */
class Widget;
} // namespace detail

/** Base class for all Widgets and Windows.
    @ingroup widgets
    @headerfile lvtk/ui/widget.hpp
*/
class LVTK_API Widget {
public:
    /** Create a blank Widget */
    Widget();
    virtual ~Widget();

    /** Set the name of this widget */
    void set_name (const std::string& name);

    /** Get the name of this widget */
    const std::string& name() const noexcept;

    /** Returns the parent widget of this one. */
    Widget* parent() const noexcept;

    /** Add a child widget. */
    void add (Widget& widget);

    /** Add a child widget. */
    template <class Wgt>
    Wgt* add (Wgt* widget) {
        add_internal (widget);
        return widget;
    }

    /** Remove a child widget.
        @param widget The widget to remove
     */
    void remove (Widget* widget);

    /** Remove a child widget.
        @param widget The widget to remove.
     */
    void remove (Widget& widget);

    /** Returns true if this widget is visible. */
    bool visible() const noexcept;

    /** Change this widget's visibility.
        @param visible True if it should be visible
     */
    void set_visible (bool visible);

    /** Show all child Widgets */
    void show_all();

    /** Request this widget be repainted. */
    void repaint();

    /** Returns true if this widget reports being opaque. */
    bool opaque() const noexcept;

    /** Returns this widget's bounding box. */
    Bounds bounds() const noexcept;

    /** Returns the xy position of this widget.
        Same as calling widget.bounds().pos()
    */
    Point<int> pos() const noexcept;

    /** Returns the x-coordinate of this widget parent space. */
    int x() const noexcept;
    /** Returns the y-coordinate of this widget parent space. */
    int y() const noexcept;
    /** Returns the width of this widget. */
    int width() const noexcept;
    /** Returns the height of this widget. */
    int height() const noexcept;

    /** Change this widget's bounds.
        
        @param x 
        @param y 
        @param width 
        @param height 
     */
    void set_bounds (int x, int y, int width, int height);

    /** Change this widget's bounds.
        
        @param b 
     */
    void set_bounds (Bounds b);

    /** Resize this widget.
     
        @param width
        @param height
    */
    void set_size (int width, int height);

    /** Returns true if the Widget can receive events at coordate xy. */
    virtual bool obstructed (int x, int y);

    /** True if this widget contains the other.
        
        @param widget The widget to test.
        @param deep If true search children recursively. default = false
    */
    bool contains (const Widget& widget, bool deep = false) const;

    /** True if xy falls within this Widget's local bounds */
    bool contains (int x, int y) const noexcept;
    /** True if pt falls within this Widget's local bounds */
    bool contains (Point<int> coord) const noexcept;
    /** True if pt falls within this Widget's local bounds */
    bool contains (Point<float> coord) const noexcept;

    /** Convert a coordinate from one widget's space to another.
        @param source The Widget to convert from.
        @param coord  A coordinate in the source.
     */
    Point<float> convert (const Widget* source, Point<float> coord) const;

    /** Convert a coordnate to view space.
        @param coord The coordnate in local space to convert
     */
    Point<int> to_view_space (Point<int> coord);

    /** Convert a coordnate to view space.
        @param coord The coordnate in local space to convert
     */
    Point<float> to_view_space (Point<float> coord);

    /** Called by the View to render this Widget.
        Invokes Widget::paint on this and all children recursively
     */
    void render (Graphics& g);

    /** Is currently focused */
    bool focused() const noexcept;

    /** Grab focus. */
    void grab_focus();

    /** Release focus. */
    void release_focus();

    /** True if this Widget owns it's View. i.e. is the top level
        widget under the OS window 
     */
    bool elevated() const noexcept;

    /** Returns the Widget underneath the given coordinate.
        @param coord The coordinate to check in local space
     */
    Widget* widget_at (Point<float> coord);

    /** Find the root widget in this tree */
    Widget* find_root() const noexcept;

    /** Find the view for this widget.
        locates the first elevated parent widget
        @returns View
     */
    View* find_view() const noexcept;

    /** Find the native handle for this Widget
        @returns uintptr_t
     */
    uintptr_t find_handle() const noexcept;

    /** Returns the current style used by this widget
        @returns Style The style used
     */
    Style& style();

    /** Cast to a LV2UI_Widget */
    operator LV2UI_Widget() const noexcept { return (LV2UI_Widget) find_handle(); }

protected:
    void set_opaque (bool opaque);

    virtual void resized() {}
    virtual void moved() {}

    virtual void paint (Graphics&) {}
    virtual void motion (const Event&) {}
    virtual void drag (const Event&) {}
    virtual void pressed (const Event&) {}
    virtual void released (const Event&) {}
    virtual void enter (const Event&) {}
    virtual void exit (const Event&) {}

    virtual bool key_down (const KeyEvent&) { return false; }
    virtual bool key_up (const KeyEvent&) { return false; }
    virtual bool text_entry (const TextEvent&) { return false; }

    virtual void children_changed() {}
    virtual void parent_structure_changed() {}
    virtual void parent_size_changed() {}
    virtual void child_size_changed (Widget* child) { ignore_unused (child); }

private:
    friend class detail::Widget;
    friend class Main;
    friend class detail::Main;
    friend class View;
    friend class detail::View;

    void add_internal (Widget*);

    std::unique_ptr<detail::Widget> impl;
    LVTK_WEAK_REFABLE (Widget)
};

/** A Weak Reference to a Widget. */
using WidgetRef = WeakRef<Widget>;
} // namespace lvtk

/** @} */
