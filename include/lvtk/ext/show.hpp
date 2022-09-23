// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/idle.hpp"

#include <lv2/ui/ui.h>

namespace lvtk {

/** Adds LV2UI_Show support to your UI instance.  This interface inherits
    from Idle. In other words, don't use Idle + Show together, just use Show.

    @ingroup ext
    @headerfile lvtk/ext/show.hpp
 */
template <class I>
struct Show : Idle<I> {
    /** @private */
    Show (const FeatureList& f) : Idle<I> (f) {}

    /** Called by the host to show your UI. Return non-zero on error */
    int show() { return 0; }

    /** Called by the host to hide your UI. Return non-zero on error */
    int hide() { return 0; }

protected:
    inline static void map_extension_data (ExtensionMap& dmap) {
        Idle<I>::map_extension_data (dmap); // idle required for show Hide
        static const LV2UI_Show_Interface _show_iface = { _show, _hide };
        dmap[LV2_UI__showInterface]                   = &_show_iface;
    }

private:
    static int _show (LV2UI_Handle ui) { return (static_cast<I*> (ui))->show(); }
    static int _hide (LV2UI_Handle ui) { return (static_cast<I*> (ui))->hide(); }
};

} // namespace lvtk
