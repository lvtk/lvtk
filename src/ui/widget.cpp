// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "lvtk/ui/widget.hpp"
#include "lvtk/ui/main.hpp"
#include "lvtk/ui/style.hpp"
#include <algorithm>

// =================== widget debugging ===================//
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

static Point<float> coord_from_parent_space (const Widget& widget,
                                             const Point<float> parent_coord) {
    auto result = parent_coord;
    if (widget.elevated())
        return result;
    result -= widget.bounds().pos().as<float>();
    return result;
}

static Point<float> coord_from_parent_space (const Widget* parent, const Widget& target, Point<float> parent_coord) {
    auto direct = target.parent();
    if (direct == parent)
        return coord_from_parent_space (target, parent_coord);

    return coord_from_parent_space (target,
                                    coord_from_parent_space (parent, *direct, parent_coord));
}

static Point<float> coord_to_parent_space (const Widget& widget,
                                           const Point<float> local_coord) {
    auto result = local_coord;
    if (widget.elevated())
        return result;
    result += widget.bounds().pos().as<float>();
    return result;
}

static Point<float> convert_coord (const Widget* tgt, Widget* src, Point<float> pt) {
    while (src != nullptr) {
        if (src == tgt)
            return pt;

        /* is source parent of tgt deep */
        auto child = tgt;
        while (child != nullptr) {
            child = child->parent();
            if (child == src) {
                return coord_from_parent_space (src, *tgt, pt);
            }
        }

        pt = coord_to_parent_space (*src, pt);
        src = src->parent();
    }

    if (tgt == nullptr)
        return pt;

    auto root = tgt->find_root();
    pt = coord_from_parent_space (*root, pt);
    if (root == tgt)
        return pt;

    return coord_from_parent_space (root, *tgt, pt);
}

} // namespace detail

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
    if (auto view = find_view())
        view->__pugl_post_redisplay();
}

//=============================================================================
void Widget::set_bounds (int x, int y, int width, int height) {
    const bool was_moved = _bounds.x != x || _bounds.y != y;
    const bool was_resized = _bounds.width != width || _bounds.width != y;

    _bounds.x = x;
    _bounds.y = y;
    _bounds.width = width;
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

    // TODO: zorder

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
        if (child->visible() && detail::test_pos (*child, detail::coord_from_parent_space (*child, pos))) {
            return true;
        }
    }

    return false;
}

Widget* Widget::widget_at (Point<float> pos) {
    if (_visible && detail::test_pos (*this, pos)) {
        for (auto child : _widgets) {
            if (auto c2 = child->widget_at (detail::coord_from_parent_space (*child, pos)))
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

Point<double> Widget::convert (Widget& source, Point<double> pt) const {
    return detail::convert_coord (this, &source, pt.as<float>()).as<double>();
}

void Widget::render (Graphics& g) {
    render_internal (g);
}

//=================================================================
bool Widget::contains (Widget& widget, bool deep) {
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
    return contains (Point<int> { x, y }.as<double>());
}

bool Widget::contains (Point<int> pt) const noexcept {
    return contains (pt.as<double>());
}

bool Widget::contains (Point<double> pt) const noexcept {
    return _bounds.at (0, 0).as<double>().contains (pt);
}

//=================================================================
void Widget::render_internal (Graphics& g) {
    paint (g);

    for (auto cw : _widgets) {
        if (! cw->visible())
            continue;
        g.save();
        g.translate (cw->bounds().pos());
        cw->render (g);
        g.restore();
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
