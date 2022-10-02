// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cassert>

#include <lvtk/ui/point.hpp>
#include <lvtk/ui/rectangle.hpp>

#ifndef LVTK_WIDGET_USE_CLIPPING
#    define LVTK_WIDGET_USE_CLIPPING 0
#endif

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

namespace detail {

template <typename T>
static int round_int (T v) { return static_cast<int> (v); }

static bool test_pos (lvtk::Widget& widget, Point<float> pos) {
    auto ipos = pos.as<int>();
    return widget.bounds().at (0, 0).contains (ipos)
           && widget.obstructed (ipos.x, ipos.y);
}

class View;

class Widget {
public:
    using Owner = lvtk::Widget;

    Widget (lvtk::Widget& o) : owner (o) {}
    ~Widget() {}

    void grab_focus();
    void release_focus();

    void render_internal (Graphics& g);
    void repaint_internal (Bounds b);

    void notify_structure_changed();
    void notify_children_changed();
    void notify_moved_resized (bool was_moved, bool was_resized);

private:
    friend class lvtk::Widget;
    friend class lvtk::Main;
    friend class detail::Main;
    friend class lvtk::View;
    friend class detail::View;

    lvtk::Widget& owner;
    std::string name;
    lvtk::Widget* parent = nullptr;
    std::unique_ptr<lvtk::View> view;
    std::vector<lvtk::Widget*> widgets;
    Rectangle<int> bounds;
    bool visible { false };
    bool opaque { false };

    static bool clip_widgets_blocking (const lvtk::Widget& w, Graphics& g, const Rectangle<int> cr, Point<int> delta) {
        int nclips = 0;

        for (int i = w.impl->widgets.size(); --i >= 0;) {
            auto& cw = *w.impl->widgets[i];

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

    static void render_child (lvtk::Widget& cw, Graphics& g) {
        g.translate (cw.pos());
        cw.render (g);
    }

    static void render_all_unclipped (lvtk::Widget& widget, Graphics& g) {
        auto cb = g.last_clip();

        g.save();
        widget.paint (g);
        g.restore();

        for (auto cw : widget.impl->widgets) {
            if (! cw->visible())
                continue;

            g.save();

            if (cb.intersects (cw->bounds())) {
                render_child (*cw, g);
            }

            g.restore();
        }
    }

    static void render_all (lvtk::Widget& widget, Graphics& g) {
        auto cb = g.last_clip();

        g.save();
        if (! (clip_widgets_blocking (widget, g, cb, {}) && g.clip_empty())) {
            widget.paint (g);
        }
        g.restore();

        for (auto cw : widget.impl->widgets) {
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

} // namespace detail
} // namespace lvtk
