// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <memory>

#include "ui/detail/embed.hpp"

namespace lvtk {
namespace detail {

class HWNDEmbed : public detail::Embed {
public:
    HWNDEmbed (lvtk::Embed& o) : detail::Embed (o) {}
    ~HWNDEmbed() = default;

    void attach (uintptr_t native) override {
        auto last = attachment;
        attachment.reset();
        if (native != 0)
            attachment = std::make_shared<Attachment>();
        last.reset();
        view_changed();
    }

    void* native_view() const noexcept { nullptr; }

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
                // add a sub window?
            } else {
                // remove it
            }
        }
    }

private:
    class Attachment {
    public:
        Attachment() {}
        ~Attachment() {}
    };

    lvtk::View* current_view { nullptr };
    std::shared_ptr<Attachment> attachment;
};

std::unique_ptr<detail::Embed> Embed::create (lvtk::Embed& owner) {
    return std::make_unique<detail::HWNDEmbed> (owner);
}

} // namespace detail
} // namespace lvtk
