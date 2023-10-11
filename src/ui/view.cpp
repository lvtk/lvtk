// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/view.hpp>

#include "detail/main.hpp"
#include "detail/view.hpp"

namespace lvtk {

namespace detail {

View::View (lvtk::View& o, lvtk::Main& m, lvtk::Widget& w)
    : owner (o), main (m), widget (w), buttons(), keyboard() {
    view       = puglNewView (m.impl->world);
    pugl_scale = puglGetScaleFactor (view);
    puglSetSizeHint (view, PUGL_MIN_SIZE, (int) pugl_scale, (int) pugl_scale);
    puglSetSizeHint (view,
                     PUGL_DEFAULT_SIZE,
                     static_cast<PuglSpan> (scale_factor() * std::max (1, w.width())),
                     static_cast<PuglSpan> (scale_factor() * std::max (1, w.height())));
    puglSetHandle (view, this);
    puglSetEventFunc (view, dispatch);
}

View::~View() {
    puglStopTimer (view, 0);
    puglFreeView (view);
    view = nullptr;
}

} // namespace detail

View::View (Main& m, Widget& w) {
    impl = std::make_unique<detail::View> (*this, m, w);
    _weak_status.reset (this);
    m.impl->views.push_back (this);
}

View::~View() {
    _weak_status.reset();
    detail::erase (main().impl->views, this);
    impl.reset();
}

void View::set_backend (uintptr_t b) {
    impl->set_backend (b);
}

void View::set_view_hint (int hint, int value) {
    impl->set_view_hint (hint, value);
}

uintptr_t View::handle() { return puglGetNativeView (impl->view); }
float View::scale_factor() const noexcept { return impl->scale_factor(); }

void View::set_visible (bool visible) {
    if (visible)
        puglShow (impl->view, PUGL_SHOW_RAISE);
    else
        puglHide (impl->view);
}

bool View::visible() const { return puglGetVisible (impl->view); }

void View::set_size (int width, int height) {
    VIEW_DBG ("set_size: " << width << "x" << height);
    auto status = puglSetSize (impl->view,
                               static_cast<unsigned int> (width * scale_factor()),
                               static_cast<unsigned int> (height * scale_factor()));
    switch (status) {
        case PUGL_SUCCESS:
            break;
        default:
            break;
    }
}

Rectangle<int> View::bounds() const {
    auto f = puglGetFrame (impl->view);
    return {
        static_cast<int> ((float) f.x / scale_factor()),
        static_cast<int> ((float) f.y / scale_factor()),
        static_cast<int> ((float) f.width / scale_factor()),
        static_cast<int> ((float) f.height / scale_factor())
    };
}

void View::set_bounds (Bounds b) {
    VIEW_DBG2 ("set_bounds: " << b.str());
    b *= scale_factor();
    puglSetFrame (impl->view, detail::frame (b));
}

Main& View::main() noexcept { return impl->main; }
Style& View::style() noexcept { return impl->main.style(); }
const Style& View::style() const noexcept { return impl->main.style(); }
Widget& View::widget() noexcept { return impl->widget; }
const Widget& View::widget() const noexcept { return impl->widget; }

void View::elevate (Widget& widget, ViewFlags flags) {
    impl->main.elevate (widget, flags, *this);
}

void View::repaint (Bounds area) {
#if ! LVTK_WIDGET_USE_CLIPPING
    area = {};
#endif
    if (area.empty()) {
        puglPostRedisplay (impl->view);
    } else {
        area *= scale_factor();
        puglPostRedisplayRect (impl->view, { (PuglCoord) area.x, (PuglCoord) area.y, (PuglSpan) area.width, (PuglSpan) area.height });
    }
}

void View::render (DrawingContext& ctx) {
    Graphics g (ctx);
    impl->widget.render (g);
}

boost::signals2::connection View::connect_idle (const IdleSlot& slot) {
    return impl->sig_idle.connect (slot);
}

} // namespace lvtk
