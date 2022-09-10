// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "lvtk/ui/widget.hpp"
#include "lvtk/ui/main.hpp"
#include "lvtk/ui/style.hpp"
#include <algorithm>

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
    if (was_moved || was_resized)
        resized();
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
    if (nullptr == widget)
        return;
    _widgets.push_back (widget);
    widget->_parent = this;
    return;
}

void Widget::remove (Widget* widget) {
    auto it = std::find (_widgets.begin(), _widgets.end(), widget);
    if (it != _widgets.end())
        _widgets.erase (it);
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

bool Widget::contains (int x, int y) const noexcept {
    return contains (Point<int> { x, y }.as<double>());
}

bool Widget::contains (Point<int> pt) const noexcept {
    return contains (pt.as<double>());
}

bool Widget::contains (Point<double> pt) const noexcept {
    return _bounds.at (0, 0).as<double>().contains (pt);
}

} // namespace lvtk
