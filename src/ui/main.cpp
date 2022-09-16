// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <lvtk/ui/button.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/style.hpp>

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

namespace lvtk {
namespace detail {
static inline PuglWorldType world_type (Mode mode) {
    if (mode == Mode::PROGRAM)
        return PUGL_PROGRAM;           
    else if (mode == Mode::MODULE)
        return PUGL_MODULE;
    return PUGL_MODULE;
}

static inline PuglWorldFlags world_flags() {
    return 0;
}

class DefaultStyle : public Style {
public:
    DefaultStyle() {
        set_color (ColorID::BUTTON_BASE, Color (0x464646ff));
        set_color (ColorID::BUTTON_ON, Color (0x252525ff));
        set_color (ColorID::BUTTON_TEXT_OFF, Color (0xeeeeeeff));
        set_color (ColorID::BUTTON_TEXT_ON, Color (0xddddddff));
    }

    ~DefaultStyle() {}

    void draw_button_shape (Graphics& g, Button& w, bool highlight, bool down) override {
        auto bc = w.toggled() ? find_color (ColorID::BUTTON_ON) : find_color (ColorID::BUTTON_BASE);
        if (highlight || down) {
            if (! down)
                bc = bc.brighter (-0.015f);
            else
                bc = bc.brighter (-0.035f);
        }

        g.set_color (bc);
        g.fill_rect (w.bounds().at (0));
    }

    void draw_button_text (Graphics& g, TextButton& w, bool highlight, bool down) override {
        auto c = find_color (w.toggled() ? ColorID::BUTTON_TEXT_ON : ColorID::BUTTON_TEXT_OFF);
        if (highlight || down)
            c = c.brighter (0.05);
        g.set_color (c);

        auto r = w.bounds().at (0).as<float>();
        g.draw_text (w.text(), r, Alignment::CENTERED);
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

std::unique_ptr<View> Main::create_view (Widget& widget, ViewFlags flags, uintptr_t parent) {
    auto view = _backend->create_view (*this, widget);
    if (! view)
        return nullptr;

    const bool transient = false;

    if (view && parent)
        view->set_parent (parent, transient);

    view->set_view_hint (PUGL_RESIZABLE, (int) (flags & ViewFlag::RESIZABLE));
    // if (flags & ViewFlag::POPUP)
    //     view->set_view_hint (PUGL_VIEW_TYPE, PUGL_POPUP_MENU_VIEW);

    return view;
}
void Main::elevate (Widget& widget, ViewFlags flags, uintptr_t parent) {
    if (widget._view != nullptr)
        return;

    auto view = create_view (widget, flags, parent);
    // bail out conditions?

    view->set_bounds (widget.bounds());
    view->realize();
    view->set_visible (widget.visible());
    widget._view = std::move (view);
    widget.notify_structure_changed();
}

void Main::elevate (Widget& widget, ViewFlags flags, View& parent) {
    elevate (widget, flags, parent.handle());
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
