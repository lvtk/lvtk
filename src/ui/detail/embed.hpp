// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/embed.hpp>
#include <lvtk/ui/rectangle.hpp>

#include <iostream>

namespace lvtk {
namespace detail {

class Embed {
public:
    class Proxy : public lvtk::Widget {
    public:
        Proxy() {}
        ~Proxy() {}
    };

    virtual ~Embed() = default;

    void resized() {
        if (! proxy)
            return;

        if (auto pv = proxy->find_view()) {
            Bounds r = owner.bounds();
            auto pos = owner.to_view_space (r.pos());
            r.x      = pos.x;
            r.y      = pos.y;

            if (! r.empty()) {
                pv->set_bounds (r);
                proxy->set_size (r.width, r.height);
            }
        }
    }

    virtual void create_proxy_view() {
        if (proxy != nullptr)
            return;

        if (auto owner_view = owner.find_view()) {
            create_proxy();
            if (proxy) {
                owner_view->elevate (*proxy, 0);
                if (auto pv = proxy->find_view()) {
                    proxy->set_size (240, 360);
                    proxy->set_visible (true);
                    pv->set_visible (true);
                }
            }
        } else {
            // std::clog << "[embed] window: owner didn't get parent view\n";
        }
    }

    inline lvtk::View* proxy_view() const noexcept {
        return proxy != nullptr ? proxy->find_view() : nullptr;
    }

    inline void close_proxy() {
        if (proxy) {
            proxy->set_visible (false);
            proxy.reset();
        }
    }

    inline void create_proxy() {
        close_proxy();
        proxy = std::make_unique<Proxy>();
        proxy->set_size (1, 1);
    }

protected:
    Embed (lvtk::Embed& o) : owner (o) {}
    lvtk::Embed& owner;
    std::unique_ptr<Proxy> proxy;

private:
    friend class lvtk::Embed;
    /** @internal Platform specific factory function. */
    static std::unique_ptr<detail::Embed> create (lvtk::Embed&);
};

} // namespace detail
} // namespace lvtk
