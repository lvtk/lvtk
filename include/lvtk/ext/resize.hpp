// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/extension.hpp"

#include <lv2/ui/ui.h>

namespace lvtk {

/** Support for UI Resizing
    @headerfile lvtk/ext/resize.hpp
    @ingroup ext
*/
template <class I>
struct Resize : Extension<I> {
    /** @private */
    Resize (const FeatureList& features) {
        if (auto* data = features.data (LV2_UI__resize))
            resize = (LV2UI_Resize*) data;
    }

    /** Called from the <em>plugin</em> to notify the host of size change
        @returns non-zero on error
     */
    int notify_size (int width, int height) {
        return (resize != nullptr) ? resize->ui_resize (resize->handle, width, height)
                                   : 1;
    }

    /** Called by the <em>host</em> to request a new UI size
        @return non-zero on error
     */
    int size_requested (int width, int height) { return 0; }

protected:
    /** @private */
    static void map_extension_data (ExtensionMap& emap) {
        static LV2UI_Resize _resize = { nullptr, _ui_resize };
        emap[LV2_UI__resize]        = &_resize;
    }

private:
    LV2UI_Resize* resize = nullptr;
    inline static int _ui_resize (LV2UI_Feature_Handle handle, int width, int height) {
        return (static_cast<I*> (handle))->size_requested (width, height);
    }
};

} // namespace lvtk
