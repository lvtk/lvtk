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

/** @defgroup instance_access Instance Access 
    Access to the plugin instance
*/

#pragma once

#include <lv2/instance-access/instance-access.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {

/** Wrap the Instance Access host feature.
 
    Use these on the stack and all set() passing the appropriate feature.
    @headerfile lvtk/ext/instance_access.hpp
    @ingroup instance_access
*/
struct InstanceHandle final : FeatureData<Handle, Handle> {
    InstanceHandle() : FeatureData (LV2_INSTANCE_ACCESS_URI) {}
};

/** Adds LV2 Instance Access support to your UI
    @headerfile lvtk/ext/instance_access.hpp
    @ingroup instance_access 
*/
template<class I>
struct InstanceAccess : NullExtension
{
    /** @private */
    InstanceAccess (const FeatureList& features) {
        for (const auto& f : features)
            if (instance.set (f))
                break;
    }

    /** @returns the LV2_Handle of the plugin if available, otherwise nullptr */
    Handle plugin_instance() const { return instance.get(); }

private:
    InstanceHandle instance;
};

}
