// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lv2/ui/ui.h>

#include "lvtk/ui/graphics.hpp"
#include "lvtk/ui/input.hpp"
#include "lvtk/ui/view.hpp"
#include "lvtk/weak_ref.hpp"

#ifndef LVTK_WIDGET_USE_CLIPPING
#    define LVTK_WIDGET_USE_CLIPPING 0
#endif

namespace lvtk {

class Widget {
public:
    Widget();
    virtual ~Widget();

    Widget* parent() const noexcept { return _parent; }

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

    /** Request this widget be repainted. */
    void repaint();

    /** Returns true if this widget reports being opaque. */
    bool opaque() const noexcept { return _opaque; }

    /** Returns this widget's bounding box. */
    Bounds bounds() const noexcept { return _bounds; }

    /** Returns the xy position of this widget.
        Same as calling widget.bounds().pos()
    */
    Point<int> pos() const noexcept { return { _bounds.x, _bounds.y }; }

    /** Returns the x-coordinate of this widget parent space. */
    int x() const noexcept { return _bounds.x; }
    /** Returns the y-coordinate of this widget parent space. */
    int y() const noexcept { return _bounds.y; }
    /** Returns the width of this widget. */
    int width() const noexcept { return _bounds.width; }
    /** Returns the height of this widget. */
    int height() const noexcept { return _bounds.height; }

    /** Change this widget's bounds
        
        @param x 
        @param y 
        @param width 
        @param height 
     */
    void set_bounds (int x, int y, int width, int height);

    /** Change this widget's bounds
        
        @param b 
     */
    void set_bounds (Bounds b);

    /** Resize this widget
     
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

    //=========================================================================
    /** Called by the View to render this Widget.
        Invokes Widget::paint on this and all children recursively
     */
    void render (Graphics& g);

    //=========================================================================
    /** True if this Widget owns it's View. i.e. is the top level
        widget under the OS window 
     */
    bool elevated() const noexcept { return _view != nullptr; }

    //=========================================================================
    /** Returns the Widget underneath the given coordinate.
        @param coord The coordinate to check in local space
     */
    Widget* widget_at (Point<float> coord);

    //=========================================================================
    Widget* find_root() const noexcept;
    ViewRef find_view() const noexcept;
    uintptr_t find_handle() const noexcept;
    operator LV2UI_Widget() const noexcept { return (LV2UI_Widget) find_handle(); }

    Style& style();

    //=========================================================================
    // things for debugging only
    // this kind of access into the widget will eventually be stable with
    // an obverver system + object query system.
    std::string __name;
    const std::vector<Widget*> __widgets() const noexcept { return _widgets; }
    // end debug things
    //=========================================================================

protected:
    void set_opaque (bool opaque);

    virtual void resized() {}
    virtual void moved() {}

    virtual void paint (Graphics&) {}
    virtual void motion (InputEvent) {}
    virtual void pressed (InputEvent) {}
    virtual void released (InputEvent) {}
    virtual void pointer_in (InputEvent ev) {}
    virtual void pointer_out (InputEvent ev) {}

    virtual void children_changed() {}
    virtual void parent_structure_changed() {}
    virtual void parent_size_changed() {}
    virtual void child_size_changed (Widget* child) {}

private:
    friend class Main;
    friend class View;
    Widget* _parent = nullptr;
    std::unique_ptr<View> _view;
    std::vector<Widget*> _widgets;
    Rectangle<int> _bounds;
    bool _visible { false };
    bool _opaque { false };

    void add_internal (Widget*);
    void render_internal (Graphics&);
    void repaint_internal (Bounds);

    void notify_structure_changed();
    void notify_children_changed();
    void notify_moved_resized (bool, bool);

    struct Render;
    LVTK_WEAK_REFABLE (Widget)
};

using WidgetRef = WeakRef<Widget>;
} // namespace lvtk
