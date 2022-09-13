/* 
    Copyright (c) 2019, Michael Fisher <mfisher@lvtk.org>

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

#include "lvtk/ext/ui/idle.hpp"

#include <lv2/ui/ui.h>

namespace lvtk {

/** Adds LV2UI_Show support to your UI instance.  This interface inherits
    from Idle. In other words, don't use Idle + Show together, just use Show.

    @ingroup ui
    @headerfile lvtk/ext/ui/show.hpp
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
