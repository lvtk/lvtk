// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/extension.hpp"

#include <lv2/ui/ui.h>

namespace lvtk {

/** Support for UI Touch
    @ingroup ext
    @headerfile lvtk/ext/touch.hpp
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
        @param port The Port Index
        @param grabbed True if grabbed or false if not
     */
    void touch (uint32_t port, bool grabbed) {
        if (ui_touch != nullptr)
            ui_touch->touch (ui_touch->handle, port, grabbed);
    }

private:
    LV2UI_Touch* ui_touch = nullptr;
};

} // namespace lvtk
