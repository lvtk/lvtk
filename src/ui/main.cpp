// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "lvtk/ui/main.hpp"
#include "lvtk/ui/widget.hpp"

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

namespace lvtk {
namespace detail {
static inline PuglWorldType world_type (Mode mode) {
    switch (mode) {
        case lvtk::Mode::PROGRAM:
            return PUGL_PROGRAM;
            break;
        case lvtk::Mode::MODULE:
            return PUGL_MODULE;
            break;
    }
    return PUGL_MODULE;
}

static inline PuglWorldFlags world_flags() {
    return 0; //PUGL_WORLD_THREADS;
}

} // namespace detail

Main::Main (Mode m, std::unique_ptr<Backend> b)
    : _world ((uintptr_t) puglNewWorld (detail::world_type (m), detail::world_flags())),
      _backend (std::move (b)),
      _mode (m) {}

Main::~Main() {}

void Main::loop (double timeout) {
    auto status = puglUpdate ((PuglWorld*) _world, timeout);
    (void) status;
}

void Main::quit() {
    __quit_flag = true;
}

std::unique_ptr<View> Main::create_view (Widget& widget, uintptr_t parent) {
    auto view = _backend->create_view (*this, widget);
    if (view && parent)
        view->set_parent (parent);
    return view;
}

void Main::elevate (Widget& widget, uintptr_t parent) {
    if (widget._view != nullptr)
        return;

    auto view = create_view (widget, parent);
    // bail out conditions?

    view->set_size (widget.width(), widget.height());
    view->realize();
    view->set_visible (widget.visible());
    widget._view = std::move (view);
    widget.__sig_elevated();
    for (auto w : widget._widgets)
        w->__sig_elevated();
}

void* Main::handle() const noexcept {
    return puglGetNativeWorld ((PuglWorld*) _world);
}

} // namespace lvtk
