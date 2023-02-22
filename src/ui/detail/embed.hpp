
#pragma once

#include <lvtk/ui/embed.hpp>
#include <lvtk/ui/rectangle.hpp>

#include <iostream>

namespace lvtk {
namespace detail {

Rectangle<float> native_geometry (ViewRef);

class Embed {
public:
    class Proxy : public lvtk::Widget {
    public:
        Proxy() {}
        ~Proxy() {}
    };

    virtual ~Embed() = default;

    virtual void attach (uintptr_t) {}
    virtual void resized() {}

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
                }
            }
        } else {
            // std::clog << "[embed] window: owner didn't get parent view\n";
        }
    }

    /** Returns the view of the main lvtk::Widget that owns the impl. */
    inline lvtk::View* owner_view() const noexcept {
        return owner.find_view();
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
