// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ext/extension.hpp>

#include <lv2/ui/ui.h>

namespace lvtk {

/** Adds the idle interface to your UI instance
    @headerfile lvtk/ext/idle.hpp
    @ingroup ext
 */
template <class I>
struct Idle : Extension<I> {
    /** @private */
    Idle (const FeatureList&) {}

    /** Called repeatedly by the host to drive your UI.  Return non-zero
        to stop receiving callbacks.

        @returns one by default, so you must override this to be useful.
     */
    inline int idle() { return 1; }

protected:
    inline static void map_extension_data (ExtensionMap& dmap) {
        static const LV2UI_Idle_Interface _idle_interface = { _idle };
        dmap[LV2_UI__idleInterface]                       = &_idle_interface;
    }

private:
    static int _idle (LV2UI_Handle ui) { return (static_cast<I*> (ui))->idle(); }
};

} // namespace lvtk
