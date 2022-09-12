// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/button.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/style.hpp>

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

class DefaultStyle : public Style {
public:
    DefaultStyle() {
        set_color ((int) ColorIDs::BUTTON_BASE, Color (0x333333ff));
        set_color ((int) ColorIDs::BUTTON_ON, Color (0x252525ff));
        set_color ((int) ColorIDs::BUTTON_TEXT_OFF, Color (0xeeeeeeff));
        set_color ((int) ColorIDs::BUTTON_TEXT_ON, Color (0xddddddff));
    }

    ~DefaultStyle() {}

    void draw_button_shape (Graphics& g, Button& w, bool highlight, bool down) override {
        auto bc = highlight || down ? find_color (ColorIDs::BUTTON_BASE)
                                    : find_color (ColorIDs::BUTTON_BASE).brighter (0.1);
        g.set_color (w.toggled() ? find_color (ColorIDs::BUTTON_ON) : bc);
        g.fill_rect (w.bounds().at (0));
    }

    void draw_button_text (Graphics& g, TextButton& w, bool highlight, bool down) override {
        auto c = find_color (w.toggled() ? ColorIDs::BUTTON_TEXT_ON : ColorIDs::BUTTON_TEXT_OFF);
        if (highlight || down)
            c = c.brighter (0.05);
        g.set_color (c);
        g.text (w.text(), 10, 12);
    }
};

} // namespace detail

Main::Main (Mode m, std::unique_ptr<Backend> b)
    : _world ((uintptr_t) puglNewWorld (detail::world_type (m), detail::world_flags())),
      _backend (std::move (b)),
      _mode (m) {
    _style.reset (new detail::DefaultStyle());
}

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
    widget.notify_structure_changed();
}

void Main::elevate (Widget& widget, View& parent) {
    elevate (widget, parent.handle());
}

View* Main::find_view (Widget& widget) const noexcept {
    for (auto view : _views) {
        if (view == widget._view.get())
            return view;
        if (view->_widget.contains (widget, true))
            return view;
    }
    return nullptr;
}

void* Main::handle() const noexcept {
    return puglGetNativeWorld ((PuglWorld*) _world);
}

} // namespace lvtk
