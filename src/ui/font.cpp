// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/font.hpp>

namespace lvtk {

Font Font::with_style (uint8_t flags) const noexcept {
    Font f;
    *f._state       = *_state;
    f._state->flags = flags;
    return f;
}

Font Font::with_height (float height) const noexcept {
    Font f;
    *f._state        = *_state;
    f._state->height = height;
    return f;
}

} // namespace lvtk
