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

#include <lvtk/lvtk.hpp>
#include <lv2/lv2plug.in/ns/ext/instance-access/instance-access.h>

namespace lvtk {

/** Wrap the Instance Access host feature.
 
    Use these on the stack and all set_feature() passing the appropriate feature.
    @headerfile lvtk/ext/instance_access.hpp
*/
struct InstanceAccess {
    /** ctor */
    InstanceAccess() = default;

    /** Get the plugin instance
        @returns The LV2 plugin instance handle or nullptr if not available
     */
    Handle get_instance() const { return handle; }

    /** Assign the LV2_Handle by LV2 Feature */
    void set_feature (const Feature& feature) {
        if (strcmp (LV2_INSTANCE_ACCESS_URI, feature.URI) == 0)
            handle = reinterpret_cast<Handle> (feature.data);
    }

    private:
        /** @internal Feature Data passed from host */
        Handle handle { nullptr };
};

} /* namespace lvtk */
