// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <lvtk/ui/embed.hpp>
#include <lvtk/ui/main.hpp>

namespace lvtk {
namespace detail {

class Embed final {
public:
    lvtk::Embed& owner;
    ViewRef owner_view;

    class Proxy : public lvtk::Widget {
    public:
        Proxy() {}
        ~Proxy() {}
    };

    Embed (lvtk::Embed& x)  : owner (x) {
        create_window();
    }

    ~Embed() {
    }

    void create_proxy() {
        proxy = std::make_unique<Proxy>();
        proxy->set_size (1, 1);
    }

    void create_window() {
        if (proxy != nullptr)
            return;

        owner_view = owner.find_view();

        if (owner_view != nullptr) {
            create_proxy();
            owner_view->elevate (*proxy, 0);
            if (auto pv = proxy->find_view()) {
                proxy->set_visible (true);
            }
        } else {
            std::clog << "[embed] window: owner didn't get parent view\n";
        }
    }

    void resized() {
        if (! proxy)
            return;

        if (auto pv = proxy->find_view()) {
            auto r = pv->bounds();
            r.x = owner.x();
            r.y = owner.y();
            r.width = owner.width();
            r.height = owner.height();
            pv->set_bounds (r);
        }
    }

    std::unique_ptr<Proxy> proxy;
};

} /* namespace detail */

Embed::Embed()
    : impl (std::make_unique<detail::Embed> (*this)) {
    set_opaque (true);
}

Embed::~Embed() {
    impl.reset();
}

ViewRef Embed::host_view() const noexcept {
    return (impl != nullptr && impl->proxy != nullptr)
               ? impl->proxy->find_view()
               : nullptr;
}

void Embed::paint (Graphics& g) {
    g.set_color (0x040404ff);
    g.fill_rect (bounds().at (0));
}

void Embed::resized() {
    impl->resized();
}

void Embed::children_changed() {
    impl->create_window();
}

void Embed::parent_structure_changed() {
    if (auto v = find_view()) {
        impl->create_window();
    }
}

}
