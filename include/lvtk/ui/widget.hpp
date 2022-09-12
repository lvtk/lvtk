// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <iostream>
#include <lv2/ui/ui.h>

#include "lvtk/ui/graphics.hpp"
#include "lvtk/ui/input.hpp"
#include "lvtk/ui/view.hpp"
#include "lvtk/weak_ref.hpp"

namespace lvtk {

class Widget {
public:
    Widget() {
        _weak_status.reset (this);
    }

    virtual ~Widget() {
        _weak_status.reset (nullptr);
    }

    // void observe (WidgetObserver&);

    Widget* parent() const noexcept { return _parent; }

    void add (Widget& widget);

    template <class Wgt>
    Wgt* add (Wgt* widget) {
        add_internal (widget);
        return widget;
    }

    void remove (Widget* widget);
    void remove (Widget& widget);

    bool visible() const noexcept;
    void set_visible (bool visible);

    void repaint();

    //==========================================
    bool opaque() const noexcept { return _opaque; }
    void set_opaque (bool opaque) { _opaque = opaque; }

    //==================
    virtual void resized() {}
    virtual void moved() {}

    virtual void paint (Graphics&) {}
    virtual void motion (InputEvent) {}
    virtual void pressed (InputEvent) {}
    virtual void released (InputEvent) {}
    //============

    Bounds bounds() const noexcept { return _bounds; }
    int x() const noexcept { return _bounds.x; }
    int y() const noexcept { return _bounds.y; }
    int width() const noexcept { return _bounds.width; }
    int height() const noexcept { return _bounds.height; }
    void set_bounds (int x, int y, int width, int height);
    void set_bounds (Bounds b);
    void set_size (int width, int height);

    /** Returns true if the Widget can receive events at coordate xy. */
    virtual bool obstructed (int x, int y);

    /** True if this widget contains the other.
        
        @param widget The widget to test.
        @param deep If true search children recursively. default = false
    */
    bool contains (Widget& widget, bool deep = false);

    /** True if xy falls within this Widget's local bounds */
    bool contains (int x, int y) const noexcept;
    /** True if pt falls within this Widget's local bounds */
    bool contains (Point<int> coord) const noexcept;
    /** True if pt falls within this Widget's local bounds */
    bool contains (Point<double> coord) const noexcept;

    /** Convert a coordinate from one widget to another.
        @param source The Widget to convert from
        @param coord  A coordinate in the source.
     */
    Point<double> convert (Widget& source, Point<double> coord) const;

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
    operator LV2UI_Widget() noexcept { return (LV2UI_Widget) find_handle(); }

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
    virtual void children_changed() {}
    virtual void parent_structure_changed() {}
    virtual void parent_size_changed() {}
    virtual void child_size_changed (Widget* child) {}

private:
    friend class Main;
    Widget* _parent = nullptr;
    std::unique_ptr<View> _view;
    std::vector<Widget*> _widgets;
    Rectangle<int> _bounds;
    bool _visible { false };
    bool _opaque { false };

    void add_internal (Widget*);
    void render_internal (Graphics&);

    void notify_structure_changed();
    void notify_children_changed();
    void notify_moved_resized (bool, bool);

    LVTK_WEAK_REFABLE (Widget)
};

using WidgetRef = WeakRef<Widget>;
} // namespace lvtk
