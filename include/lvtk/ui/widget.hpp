// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ui/graphics.hpp"
#include "lvtk/ui/input.hpp"
#include "lvtk/ui/view.hpp"
#include "lvtk/weak_ref.hpp"
#include <iostream>
#include <lv2/ui/ui.h>

namespace lvtk {

class Widget {
public:
    Widget() {
        _weak_status.reset (this);
    }

    virtual ~Widget() {
        _weak_status.reset (nullptr);
    }

    Widget* parent() const noexcept { return _parent; }

    void add (Widget& widget);
    void remove (Widget* widget);
    void remove (Widget& widget);

    bool visible() const noexcept;
    void set_visible (bool visible);

    void repaint();

    virtual void resized() {}
    virtual void paint (Graphics&) {}
    virtual void motion (InputEvent) {}
    virtual void pressed (InputEvent) {}
    virtual void released (InputEvent) {}

    Bounds bounds() const noexcept { return _bounds; }
    int width() const noexcept { return _bounds.width; }
    int height() const noexcept { return _bounds.height; }
    void set_bounds (int x, int y, int width, int height);
    void set_bounds (Bounds b);
    void set_size (int width, int height);

    /** Returns true if the Widget can receive events at coordate xy. */
    virtual bool obstructed (int x, int y);

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
        Invokes Widget::paint on this and all children.
     */
    void render (Graphics& g);

    //=========================================================================
    /** True if this Widget owns the View. i.e. is the top level
        widget under the OS window */
    bool elevated() const noexcept { return _view != nullptr; }

    //=========================================================================
    /** Returns the Widget underneath the given coordinate.
        @param coord The coordinate to check in local space
     */
    Widget* widget_at (Point<float> coord);

    //=========================================================================
    Widget* find_root() const noexcept;
    ViewRef find_view() const noexcept;
    LV2UI_Widget find_handle() const noexcept;
    operator LV2UI_Widget() noexcept { return (LV2UI_Widget) find_handle(); }

    //=========================================================================
    // things for debugging only
    std::string __name;
    const std::vector<Widget*> __widgets() const noexcept { return _widgets; }
    // end debug things
    //=========================================================================

private:
    friend class Main;
    Widget* _parent = nullptr;
    std::unique_ptr<View> _view;
    std::vector<Widget*> _widgets;
    Rectangle<int> _bounds;
    bool _visible { false };
    void render_internal (Graphics& g);
    LVTK_WEAK_REFABLE (Widget, _weak_status)
};

using WidgetRef = WeakRef<Widget>;
} // namespace lvtk
