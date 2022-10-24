// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ext/extension.hpp>

#include <lv2/instance-access/instance-access.h>

namespace lvtk {

/** Wrap the Instance Access host feature.

    Use these on the stack and all set() passing the appropriate feature.
    @headerfile lvtk/ext/instance_access.hpp
    @ingroup utility
*/
struct InstanceHandle final : FeatureData<Handle, Handle> {
    InstanceHandle() : FeatureData (LV2_INSTANCE_ACCESS_URI) {}
};

/** Adds LV2 Instance Access support to your UI
    
    @tparam Mod Your UI type
    @headerfile lvtk/ext/instance_access.hpp
    @ingroup ext
*/
template <class Mod>
struct InstanceAccess : NullExtension {
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

} // namespace lvtk
