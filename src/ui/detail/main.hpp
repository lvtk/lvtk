
#pragma once

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

#include <lvtk/ui/button.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/view.hpp>

#include "./view.hpp"

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

class Main {
public:
    Main (lvtk::Main& o, const Mode m, std::unique_ptr<lvtk::Backend> b)
        : owner (o), mode (m), world (puglNewWorld (detail::world_type (m), detail::world_flags())), backend (std::move (b)), style (std::make_unique<DefaultStyle>()) {}

    std::unique_ptr<lvtk::View> create_view (lvtk::Widget& widget, ViewFlags flags, uintptr_t parent) {
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

private:
    friend class lvtk::Main;
    friend class lvtk::View;
    friend class detail::View;

    lvtk::Main& owner;
    const Mode mode;
    PuglWorld* world { nullptr };
    std::unique_ptr<lvtk::Backend> backend;
    std::vector<lvtk::View*> views;
    std::unique_ptr<lvtk::Style> style;
};

} // namespace detail
} // namespace lvtk