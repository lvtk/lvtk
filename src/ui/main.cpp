// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <lvtk/ui/button.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/style.hpp>

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

#include "ui/detail/default_style.hpp"
#include "ui/detail/main.hpp"
#include "ui/detail/view.hpp"
#include "ui/detail/widget.hpp"

namespace lvtk {
namespace detail {

Main::Main (lvtk::Main& o, const Mode m, std::unique_ptr<lvtk::Backend> b)
    : owner (o),
      mode (m),
      world (puglNewWorld (detail::world_type (m), detail::world_flags())),
      backend (std::move (b)),
      style (std::make_unique<DefaultStyle>()) {}

std::unique_ptr<lvtk::View> Main::create_view (lvtk::Widget& widget, ViewFlags flags, uintptr_t parent) {
    auto view = backend->create_view (owner, widget);
    if (! view)
        return nullptr;

    const bool transient = false;

    if (view && parent)
        view->impl->set_parent (parent, transient);

    view->set_view_hint (PUGL_RESIZABLE, (int) (flags & lvtk::View::RESIZABLE));
    // if (flags & ViewFlag::POPUP)
    //     view->set_view_hint (PUGL_VIEW_TYPE, PUGL_POPUP_MENU_VIEW);

    return view;
}

bool Main::loop (double timeout) {
    last_update_status = puglUpdate (world, timeout);
    if (! first_loop_called) {
        first_loop_called = true;
    }
    return last_update_status == PUGL_SUCCESS;
}

} // namespace detail

Main::Main (Mode m, std::unique_ptr<Backend> b) {
    impl = std::make_unique<detail::Main> (*this, m, std::move (b));
}

Main::~Main() {}

Mode Main::mode() const noexcept { return impl->mode; }

void Main::loop (double timeout) { impl->loop (timeout); }

bool Main::running() const noexcept { return impl->running(); }

int Main::exit_code() const noexcept { return impl->exit_code.load(); }
void Main::set_exit_code (int code) { impl->exit_code.store (code); }

void Main::quit() {
    if (impl->quit_flag == true)
        return;
    impl->quit_flag = true;
}

View* Main::elevate (Widget& widget, ViewFlags flags, uintptr_t parent) {
    if (widget.impl->view != nullptr)
        return widget.impl->view.get();

    if (auto vptr = impl->create_view (widget, flags, parent))
        widget.impl->view = std::move (vptr);
    if (widget.impl->view == nullptr)
        return nullptr;

    auto& view = *widget.impl->view;
    
    view.impl->realize();
    view.set_bounds (widget.bounds());
    view.set_visible (widget.visible());
    widget.impl->notify_structure_changed();
    return widget.impl->view != nullptr ? widget.impl->view.get() : nullptr;
}

View* Main::elevate (Widget& widget, ViewFlags flags, View& parent) {
    return elevate (widget, flags, parent.handle());
}

View* Main::find_view (Widget& widget) const noexcept {
    for (auto view : impl->views) {
        if (view == widget.impl->view.get())
            return view;
        if (view->widget().contains (widget, true))
            return view;
    }
    return nullptr;
}

void* Main::handle() const noexcept {
    return puglGetNativeWorld (impl->world);
}

Style& Main::style() noexcept { return *impl->style; }
const Style& Main::style() const noexcept { return *impl->style; }

} // namespace lvtk
