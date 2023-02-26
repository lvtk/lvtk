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

    void* native_view() const noexcept { nullptr; }

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
