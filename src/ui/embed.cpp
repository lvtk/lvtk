// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "ui/detail/embed.hpp"

namespace lvtk {

Embed::Embed() {
    impl = detail::Embed::create (*this);
    impl->create_proxy_view();
}

Embed::~Embed() {
    impl.reset();
}

ViewRef Embed::proxy_view() const noexcept {
    return impl->proxy_view();
}

void Embed::paint (Graphics& g) {
    g.set_color (0x040404ff);
    g.fill_rect (bounds().at (0));
}

void Embed::resized() {
    impl->resized();
}

void Embed::children_changed() {
    impl->create_proxy_view();
}

void Embed::parent_structure_changed() {
    impl->create_proxy_view();
}

} // namespace lvtk