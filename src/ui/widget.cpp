// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <algorithm>
#include <cassert>
#include <iostream>

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/style.hpp>
#include <lvtk/ui/widget.hpp>

// =================== widget debugging =======================//
#define DBG_WIDGET 0
#if DBG_WIDGET
#    define MTRACE(i) std::clog << "[widget] " << i << std::endl
#    define DTRACE(i) std::clog << "[widget] " << i << std::endl
#else
#    define MTRACE(i)
#    define DTRACE(i)
#endif
// =================== end widget debugging ===================//

namespace lvtk {
namespace detail {
template <typename T>
static int round_int (T v) { return static_cast<int> (v); }

static bool test_pos (Widget& widget, Point<float> pos) {
    auto ipos = pos.as<int>();
    return widget.bounds().at (0, 0).contains (ipos)
           && widget.obstructed (ipos.x, ipos.y);
}

} // namespace detail

namespace convert {
#if 0 // disabled until needed
/** From pyhsical to logical coordinats. */
static Point<float> to_scaled (float scale, Point<float> coord) {
    return scale != 1.f ? coord / scale : coord;
}

/** From logical to physical coordinates. */
static Point<float> to_unscaled (float scale, Point<float> coord) {
    return scale != 1.f ? coord * scale : coord;
}
#endif

static Point<float> from_parent_space (const Widget& widget, const Point<float> parent_coord) {
    auto result = parent_coord;
    if (widget.elevated())
        return result;
    result -= widget.pos().as<float>();
    return result;
}

static Point<float> from_ancestor_space (const Widget* parent, const Widget& target, Point<float> parent_coord) {
    auto direct = target.parent();
    if (direct == parent)
        return from_parent_space (target, parent_coord);

    return from_parent_space (target, from_ancestor_space (parent, *direct, parent_coord));
}

static Point<float> to_parent_space (const Widget& widget,
                                     const Point<float> local_coord) {
    auto result = local_coord;
    if (widget.elevated())
        return result;
    result += widget.pos().as<float>();
    return result;
}

static Point<float> coordinate (const Widget* tgt, const Widget* src, Point<float> pt) {
    while (src != nullptr) {
        if (src == tgt)
            return pt;

        if (src->contains (*tgt, true))
            return from_ancestor_space (src, *tgt, pt);

        pt  = to_parent_space (*src, pt);
        src = src->parent();
    }

    assert (src == nullptr);
    if (tgt == nullptr)
        return pt;

    auto root = tgt->find_root();
    pt        = from_parent_space (*root, pt);
    if (root == tgt)
        return pt;

    return from_ancestor_space (root, *tgt, pt);
}

} // namespace convert

Widget::Widget() { _weak_status.reset (this); }
Widget::~Widget() { _weak_status.reset (nullptr); }

//=============================================================================
bool Widget::visible() const noexcept { return _visible; }

void Widget::set_visible (bool v) {
    if (_visible != v) {
        _visible = v;
        if (_view)
            _view->set_visible (visible());
    }
}

//=============================================================================
void Widget::repaint() {
    repaint_internal (bounds().at (0));
}

//=============================================================================
void Widget::set_bounds (int x, int y, int width, int height) {
    const bool was_moved   = _bounds.x != x || _bounds.y != y;
    const bool was_resized = _bounds.width != width || _bounds.height != height;

    _bounds.x      = x;
    _bounds.y      = y;
    _bounds.width  = width;
    _bounds.height = height;

    if (visible() && was_resized)
        repaint();

    notify_moved_resized (was_moved, was_resized);
}

void Widget::set_bounds (Bounds b) { set_bounds (b.x, b.y, b.width, b.height); }

void Widget::set_size (int width, int height) {
    set_bounds (_bounds.x, _bounds.y, width, height);
}

//=============================================================================
void Widget::add (Widget& widget) {
    add_internal (&widget);
}

void Widget::add_internal (Widget* widget) {
    assert (widget != this);

    if (nullptr == widget || widget->_parent == this)
        return;

    if (widget->_parent != nullptr)
        widget->_parent->remove (widget);
    else if (widget->_view)
        widget->_view.reset();

    widget->_parent = this;

    if (widget->visible())
        widget->repaint();

    _widgets.push_back (widget);

    // child events
    widget->notify_structure_changed();

    // parent events
    notify_children_changed();
}

void Widget::remove (Widget* widget) {
    auto it = std::find (_widgets.begin(), _widgets.end(), widget);
    if (it == _widgets.end())
        return;

    _widgets.erase (it);
    widget->_parent = nullptr;

    // child events
    widget->notify_structure_changed();

    // parent events
    notify_children_changed();
}

void Widget::remove (Widget& widget) {
    remove (&widget);
}

bool Widget::obstructed (int x, int y) {
    auto pos = Point<int> { x, y }.as<float>();

    for (auto child : _widgets) {
        if (child->visible() && detail::test_pos (*child, convert::from_parent_space (*child, pos))) {
            return true;
        }
    }

    return false;
}

Widget* Widget::widget_at (Point<float> pos) {
    if (_visible && detail::test_pos (*this, pos)) {
        for (auto child : _widgets) {
            if (auto c2 = child->widget_at (convert::from_parent_space (*child, pos)))
                return c2;
        }
        return this;
    }
    return nullptr;
}

Widget* Widget::find_root() const noexcept {
    auto it = this;
    while (it && it->_parent != nullptr)
        it = it->_parent;
    return it != nullptr ? const_cast<Widget*> (it) : nullptr;
}

ViewRef Widget::find_view() const noexcept {
    auto it = this;
    while (it && it->_view == nullptr)
        it = it->_parent;
    return it != nullptr ? it->_view.get() : nullptr;
}

uintptr_t Widget::find_handle() const noexcept {
    if (auto v = find_view())
        return v->handle();
    return 0;
}

Style& Widget::style() {
    if (auto v = find_view())
        return v->style();
    assert (false);
    return find_root()->style();
}

void Widget::set_opaque (bool opaque) {
    if (_opaque == opaque)
        return;
    _opaque = opaque;
    repaint();
}

Point<float> Widget::convert (const Widget* source, Point<float> pt) const {
    return convert::coordinate (this, source, pt);
}

Point<int> Widget::to_view_space (Point<int> pt) {
    return to_view_space (pt.as<float>()).as<int>();
}

Point<float> Widget::to_view_space (Point<float> pt) {
    return convert::coordinate (nullptr, this, pt);
}

void Widget::render (Graphics& g) {
    render_internal (g);
}

//=================================================================
bool Widget::contains (const Widget& widget, bool deep) const {
    if (! deep)
        return widget._parent == this;
    auto c = &widget;
    while (c != nullptr) {
        c = c->_parent;
        if (c == this) {
            return true;
        }
    }
    return false;
}

bool Widget::contains (int x, int y) const noexcept {
    return contains (Point<int> { x, y }.as<float>());
}

bool Widget::contains (Point<int> pt) const noexcept {
    return contains (pt.as<float>());
}

bool Widget::contains (Point<float> pt) const noexcept {
    return _bounds.at (0, 0).as<float>().contains (pt);
}

//=================================================================
struct Widget::Render {
    static bool clip_widgets_blocking (const Widget& w, Graphics& g, const Rectangle<int> cr, Point<int> delta) {
        int nclips = 0;

        for (int i = w._widgets.size(); --i >= 0;) {
            auto& cw = *w._widgets[i];

            if (! cw.visible())
                continue;

            auto ncr = cr.intersection (cw.bounds());
            if (ncr.empty())
                continue;

            if (cw.opaque()) {
                g.intersect_clip (ncr + delta);
                ++nclips;
            } else {
                auto cpos = cw.pos();
                if (clip_widgets_blocking (cw, g, ncr - cpos, cpos + delta))
                    ++nclips;
            }
        }

        return nclips > 0;
    }

    static void render_child (Widget& cw, Graphics& g) {
        g.translate (cw.pos());
        cw.render (g);
    }

    static void render_all_unclipped (Widget& widget, Graphics& g) {
        auto cb = g.last_clip();

        g.save();
        widget.paint (g);
        g.restore();

        for (auto cw : widget._widgets) {
            if (! cw->visible())
                continue;

            g.save();

            if (cb.intersects (cw->bounds())) {
                render_child (*cw, g);
            }

            g.restore();
        }
    }

    static void render_all (Widget& widget, Graphics& g) {
        auto cb = g.last_clip();

        g.save();
        if (! (clip_widgets_blocking (widget, g, cb, {}) && g.clip_empty())) {
            widget.paint (g);
        }
        g.restore();

        for (auto cw : widget._widgets) {
            if (! cw->visible())
                continue;

            g.save();

            if (cb.intersects (cw->bounds())) {
                g.clip (cw->bounds().at (0));
                if (! g.clip_empty())
                    render_child (*cw, g);
            }

            g.restore();
        }
    }
};

void Widget::render_internal (Graphics& g) {
#if LVTK_WIDGET_USE_CLIPPING
    Render::render_all (*this, g);
#else
    Render::render_all_unclipped (*this, g);
#endif
}

void Widget::repaint_internal (Bounds b) {
    if (! visible())
        return;
    b = bounds().at (0).intersection (b);
    if (b.empty())
        return;

    if (elevated()) {
        _view->repaint (b);
    } else {
        if (_parent != nullptr) {
            auto p = convert::to_parent_space (*this, b.pos().as<float>());
            b.x    = detail::round_int (p.x);
            b.y    = detail::round_int (p.y);
            _parent->repaint_internal (b);
        }
    }
}

//=============================================================================
void Widget::notify_structure_changed() {
    MTRACE (__name << "::notify_structure_changed()");
    WidgetRef ref = this;
    parent_structure_changed();

    for (int i = (int) _widgets.size(); --i >= 0;) {
        _widgets[i]->notify_structure_changed();
        if (! ref.valid())
            return;
        i = std::min (i, (int) _widgets.size());
    }
}

void Widget::notify_children_changed() {
    MTRACE (__name << "::notify_children_changed()");
    // notify subclass
    children_changed();
}

void Widget::notify_moved_resized (bool was_moved, bool was_resized) {
    WidgetRef ref = this;
    if (was_moved) {
        moved();
        if (! ref.valid())
            return;
    }

    if (was_resized) {
        resized();
        if (! ref.valid())
            return;

        for (int i = (int) _widgets.size(); --i >= 0;) {
            _widgets[i]->parent_size_changed();
            if (! ref.valid())
                return;
            i = std::min (i, (int) _widgets.size());
        }
    }

    if (_parent != nullptr)
        _parent->child_size_changed (this);

    if (ref.valid()) { /* send a signal */
    };
}

//=============================================================================
#if 0
void Widget::observe (WidgetObserver& ob) {
    using slot_type = Signal<void()>::slot_type;
    slot_type myslot (&WidgetObserver::elevated, &ob);
    _sig_elevated.connect (myslot.track (&ob));
}
#endif

} // namespace lvtk
