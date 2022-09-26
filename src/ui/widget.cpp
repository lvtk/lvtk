// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <algorithm>
#include <cassert>
#include <iostream>

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/style.hpp>
#include <lvtk/ui/widget.hpp>

#include "detail/widget.hpp"

namespace lvtk {

Widget::Widget() {
    impl = std::make_unique<detail::Widget> (*this);
    _weak_status.reset (this);
}

Widget::~Widget() {
    _weak_status.reset (nullptr);
    if (auto p = parent())
        p->remove (this);
    if (impl->view)
        impl->view.reset();
}

const std::string& Widget::name() const noexcept { return impl->name; }

void Widget::set_name (const std::string& name) {
    impl->name = name;
}

bool Widget::visible() const noexcept { return impl->visible; }

void Widget::set_visible (bool v) {
    if (impl->visible != v) {
        impl->visible = v;
        if (impl->view)
            impl->view->set_visible (visible());
    }
}

//=============================================================================
void Widget::repaint() {
    impl->repaint_internal (impl->bounds.at (0));
}

bool Widget::opaque() const noexcept { return impl->opaque; }

Bounds Widget::bounds() const noexcept { return impl->bounds; }

Point<int> Widget::pos() const noexcept { return { impl->bounds.x, impl->bounds.y }; }

int Widget::x() const noexcept { return impl->bounds.x; }
int Widget::y() const noexcept { return impl->bounds.y; }
int Widget::width() const noexcept { return impl->bounds.width; }
int Widget::height() const noexcept { return impl->bounds.height; }

void Widget::set_bounds (int x, int y, int w, int h) {
    const bool was_moved   = impl->bounds.x != x || impl->bounds.y != y;
    const bool was_resized = impl->bounds.width != w || impl->bounds.height != h;

    impl->bounds.x      = x;
    impl->bounds.y      = y;
    impl->bounds.width  = w;
    impl->bounds.height = h;

    if (visible() && was_resized)
        repaint();

    impl->notify_moved_resized (was_moved, was_resized);
}

void Widget::set_bounds (Bounds b) { set_bounds (b.x, b.y, b.width, b.height); }

void Widget::set_size (int width, int height) {
    set_bounds (impl->bounds.x, impl->bounds.y, width, height);
}

Widget* Widget::parent() const noexcept { return impl->parent; }

void Widget::add (Widget& widget) {
    add_internal (&widget);
}

void Widget::add_internal (Widget* widget) {
    assert (widget != this);

    if (nullptr == widget || widget->impl->parent == this)
        return;

    if (widget->impl->parent != nullptr)
        widget->impl->parent->remove (widget);
    else if (widget->impl->view)
        widget->impl->view.reset();

    widget->impl->parent = this;

    if (widget->visible())
        widget->repaint();

    impl->widgets.push_back (widget);

    // child events
    widget->impl->notify_structure_changed();

    // parent events
    impl->notify_children_changed();
}

void Widget::remove (Widget* widget) {
    auto it = std::find (impl->widgets.begin(), impl->widgets.end(), widget);
    if (it == impl->widgets.end())
        return;

    impl->widgets.erase (it);
    widget->impl->parent = nullptr;

    // child events
    widget->impl->notify_structure_changed();

    // parent events
    impl->notify_children_changed();
}

void Widget::remove (Widget& widget) {
    remove (&widget);
}

bool Widget::obstructed (int x, int y) {
    auto pos = Point<int> { x, y }.as<float>();

    for (auto child : impl->widgets) {
        if (child->visible() && detail::test_pos (*child, convert::from_parent_space (*child, pos))) {
            return true;
        }
    }

    return false;
}

Widget* Widget::widget_at (Point<float> pos) {
    if (visible() && detail::test_pos (*this, pos)) {
        for (auto child : impl->widgets) {
            if (auto c2 = child->widget_at (convert::from_parent_space (*child, pos)))
                return c2;
        }
        return this;
    }
    return nullptr;
}

Widget* Widget::find_root() const noexcept {
    auto it = this;
    while (it && it->impl->parent != nullptr)
        it = it->impl->parent;
    return it != nullptr ? const_cast<Widget*> (it) : nullptr;
}

ViewRef Widget::find_view() const noexcept {
    auto it = this;
    while (it && it->impl->view == nullptr)
        it = it->impl->parent;
    return it != nullptr ? it->impl->view.get() : nullptr;
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

void Widget::set_opaque (bool op) {
    if (impl->opaque == op)
        return;
    impl->opaque = op;
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
    impl->render_internal (g);
}

bool Widget::elevated() const noexcept {
    return impl->view != nullptr;
}

//=================================================================
bool Widget::contains (const Widget& widget, bool deep) const {
    if (! deep)
        return widget.impl->parent == this;
    auto c = &widget;
    while (c != nullptr) {
        c = c->impl->parent;
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
    return impl->bounds.at (0, 0).as<float>().contains (pt);
}

const std::vector<Widget*>& Widget::__widgets() const noexcept {
    return impl->widgets;
}

} // namespace lvtk
