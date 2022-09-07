/*
    Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#pragma once

#include "lvtk/ext/extension.hpp"

#include <lv2/ui/ui.h>

namespace lvtk {

/** Support for UI Touch
    @ingroup ui
    @headerfile lvtk/ext/ui/touch.hpp
*/
template <class I>
struct Touch : NullExtension {
    /** @private */
    Touch (const FeatureList& features) {
        for (const auto& f : features) {
            if (f == LV2_UI__touch) {
                ui_touch = (LV2UI_Touch*) f.data;
                break;
            }
        }
    }

    /** Call this to notify the host of gesture changes.
        @returns non-zero on error
     */
    void touch (uint32_t port, bool grabbed) {
        if (ui_touch != nullptr)
            ui_touch->touch (ui_touch->handle, port, grabbed);
    }

private:
    LV2UI_Touch* ui_touch = nullptr;
};

} // namespace lvtk
