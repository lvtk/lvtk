// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <lvtk/ui/button.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/style.hpp>

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

#include "detail/main.hpp"
#include "detail/view.hpp"
#include "detail/widget.hpp"

namespace lvtk {
namespace detail {

} // namespace detail

Main::Main (Mode m, std::unique_ptr<Backend> b) {
    impl = std::make_unique<detail::Main> (*this, m, std::move (b));
}

Main::~Main() {}

Mode Main::mode() const noexcept { return impl->mode; }

void Main::loop (double timeout) {
    auto status = puglUpdate (impl->world, timeout);
    (void) status;
}

void Main::quit() {
    __quit_flag = true;
}

void Main::elevate (Widget& widget, ViewFlags flags, uintptr_t parent) {
    if (widget.impl->view != nullptr)
        return;

    auto view = impl->create_view (widget, flags, parent);
    // bail out conditions?

    view->impl->realize();
    view->set_bounds (widget.bounds());
    view->set_visible (widget.visible());
    widget.impl->view = std::move (view);
    widget.impl->notify_structure_changed();
}

void Main::elevate (Widget& widget, ViewFlags flags, View& parent) {
    elevate (widget, flags, parent.handle());
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
    uintptr_t Main::world() const noexcept { return (uintptr_t) impl->world; }
    Style& Main::style() noexcept {return *impl->style; }
    const Style& Main::style() const noexcept { return *impl->style; }

} // namespace lvtk
