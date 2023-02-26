// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <cstdint>

#include <lvtk/ui/embed.hpp>
#include <lvtk/ui/main.hpp>

#include "ui/detail/embed.hpp"

#include <X11/Xlib.h>

namespace lvtk {
namespace detail {

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

    if (window != 0)
        XGetGeometry (disp, window, &root, &x, &y, &w, &h, &bw, &d);
    return { (float) x, (float) y, float (w), float (h) };
}

class X11Embed final : public detail::Embed {
public:
    X11Embed (lvtk::Embed& x) : detail::Embed (x) {}
    ~X11Embed() {}

private:
    friend class lvtk::Embed;
    ViewRef owner_view;
};

std::unique_ptr<detail::Embed> Embed::create (lvtk::Embed& owner) {
    return std::make_unique<detail::X11Embed> (owner);
}

} // namespace detail
} // namespace lvtk
