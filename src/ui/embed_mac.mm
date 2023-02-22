
#include <lvtk/ui/rectangle.hpp>
#include <lvtk/ui/view.hpp>
#include <lvtk/ui/embed.hpp>

#include <Cocoa/Cocoa.h>
#include <iostream>

#include "ui/detail/embed.hpp"

namespace lvtk {
namespace detail {

inline static Rectangle<float> to_rect (NSView* view) {
    NSRect frame = [view frame];
    Rectangle<CGFloat> r { frame.origin.x, frame.origin.y, frame.size.width, frame.size.height };
    return r.as<float>();
}

class NSViewEmbed : public detail::Embed {
public:
    NSViewEmbed (lvtk::Embed& o) : detail::Embed (o) {}
    ~NSViewEmbed() = default;

    void attach (uintptr_t native) override {
        auto last = attachment;
        attachment.reset();
        if (native != 0)
            attachment = std::make_shared<Attachment> (static_cast<NSView*> ((void*) native));
        last.reset();
        view_changed();
    }

    NSView* native_view() const noexcept {
        return attachment != nullptr ? attachment->view : nullptr;
    }

    void resized() override {
        if (! proxy)
            return;

        if (auto pv = proxy->find_view()) {
            auto og = pv->bounds();
            Bounds r;

            r.x      = owner.x();
            r.y      = owner.y();
            r.width  = int ((float) og.width / pv->scale_factor());
            r.height = int ((float) og.height / pv->scale_factor());

            if (! r.empty()) {
                pv->set_bounds (r);
                proxy->set_bounds (r.at (0));
            }
        }
    }

    void view_changed() {
        if (attachment == nullptr)
            return;

        auto view = owner.find_view();
        if (current_view != view) {
            current_view = view;
            if (current_view != nullptr) {
                auto handle = (NSView*) current_view->handle();
                if (attachment) {
                    [handle addSubview: native_view()];
                }
            }
            else
            {
                // remove it
            }
        }
    }

private:
    class Attachment {
    public:
        Attachment (NSView* v) : view (v) {
            [view retain];
            [view setPostsFrameChangedNotifications: YES];
        }

        ~Attachment() {
            if (view != nullptr) {
                [view release];
            }
        }

        NSView* view { nullptr };
    };

    lvtk::View* current_view { nullptr };
    std::shared_ptr<Attachment> attachment;
};

std::unique_ptr<detail::Embed> Embed::create (lvtk::Embed& owner) {
    return std::make_unique<detail::NSViewEmbed> (owner);
}

} /* namespace detail */
}
