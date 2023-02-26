// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/rectangle.hpp>
#include <lvtk/ui/view.hpp>
#include <lvtk/ui/embed.hpp>

#include <Cocoa/Cocoa.h>
#include <iostream>

#include "ui/detail/embed.hpp"

namespace lvtk {
namespace detail {

class NSViewEmbed : public detail::Embed {
public:
    NSViewEmbed (lvtk::Embed& o) : detail::Embed (o) {}
    ~NSViewEmbed() { current_view = nullptr; }

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
