// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <lvtk/ui/embed.hpp>
#include <lvtk/ui/main.hpp>

#include "ui/detail/embed.hpp"

#if __linux__
#    include <X11/Xlib.h>
#else
#endif

namespace lvtk {
namespace detail {

#ifdef __linux__
static bool get_window_parent (Display* disp, uintptr_t& window, uintptr_t& _root) {
    Window root, parent, *children = nullptr;
    unsigned int num_children;

    if (! XQueryTree (disp, window, &root, &parent, &children, &num_children))
        return false;

    if (children)
        XFree ((char*) children);

    window = parent;
    _root  = root;
    return true;
}

static uintptr_t get_window_first_child (Display* disp, uintptr_t window) {
    Window root, parent, *children = nullptr;
    unsigned int num_children;

    if (! XQueryTree (disp, window, &root, &parent, &children, &num_children))
        return false;

    std::clog << "get_window_first_child: num_children = " << num_children << std::endl;

    Window child = 0;
    if (children) {
        child = children[0];
        XFree ((char*) children);
    }

    return child;
}

static Rectangle<float> native_geometry (ViewRef pv) {
    auto& main = pv->main();
    auto disp  = (Display*) main.handle();

    int x            = 0;
    int y            = 0;
    unsigned int w   = 0;
    unsigned int h   = 0;
    unsigned int bw  = 0;
    unsigned int d   = 0;
    uintptr_t root   = 0;
    uintptr_t window = get_window_first_child (disp, pv->handle());
    std::clog << "window = " << (int64_t) window << std::endl;
    if (window != 0)
        XGetGeometry (disp, window, &root, &x, &y, &w, &h, &bw, &d);
    return { (float) x, (float) y, float (w), float (h) };
}
#elif __APPLE__
#else
static Rectangle<float> native_geometry (ViewRef pv) {
    return pv->bounds().as<float>();
}
#endif

class Embed final {
public:
    class Proxy : public lvtk::Widget {
    public:
        Proxy() {}
        ~Proxy() {}
    };

    Embed (lvtk::Embed& x) : owner (x) {
        create_window();
    }

    ~Embed() {
    }

    void close_proxy() {
        if (proxy) {
            proxy->set_visible (false);
            proxy.reset();
        }
    }

    void create_proxy() {
        close_proxy();
        proxy = std::make_unique<Proxy>();
        proxy->set_size (1, 1);
    }

    void create_window() {
        if (proxy != nullptr)
            return;

        owner_view = owner.find_view();

        if (owner_view != nullptr) {
            create_proxy();
            if (proxy) {
                owner_view->elevate (*proxy, 0);
                if (auto pv = proxy->find_view()) {
                    std::clog << pv->bounds().str() << std::endl;
                    proxy->set_visible (true);
                }
            }
        } else {
            // std::clog << "[embed] window: owner didn't get parent view\n";
        }
    }

    void resized() {
        if (! proxy)
            return;

        if (auto pv = proxy->find_view()) {
            original_geometry = detail::native_geometry (pv);
            const auto& og    = original_geometry;
            Bounds r;

            r.x      = owner.x();
            r.y      = owner.y();
            r.width  = int ((float) owner.width() / pv->scale_factor());
            r.height = int ((float) owner.height() / pv->scale_factor());

            if (! r.empty()) {
                pv->set_bounds (r);
                proxy->set_bounds (r.at (0));
            }
        }
    }

private:
    friend class lvtk::Embed;
    lvtk::Embed& owner;
    ViewRef owner_view;
    Rectangle<float> original_geometry;
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

ViewRef Embed::proxy_view() const noexcept {
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
    if (nullptr != find_view()) {
        impl->create_window();
    }
}

} // namespace lvtk
