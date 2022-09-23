// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/extension.hpp"

#include <lv2/ui/ui.h>

namespace lvtk {
/** Support for UI Port Subscribe
    @ingroup ext
    @headerfile lvtk/ext/port_subscribe.hpp
*/
template <class I>
struct PortSubscribe : NullExtension {
    /** @private */
    PortSubscribe (const FeatureList& features) {
        for (const auto& f : features) {
            if (f == LV2_UI__portSubscribe) {
                port_subscribe = *(LV2UI_Port_Subscribe*) f.data;
                break;
            }
        }
    }

    /** Subscribe to port events */
    uint32_t subscribe (uint32_t port, uint32_t protocol, const LV2_Feature* const* features) const {
        return (port_subscribe.handle != nullptr)
                   ? port_subscribe.subscribe (port_subscribe.handle, port, protocol, features)
                   : 1;
    }

    /** Unsubscribe from port events */
    uint32_t unsubscribe (uint32_t port, uint32_t protocol, const LV2_Feature* const* features) const {
        return (port_subscribe.handle != nullptr)
                   ? port_subscribe.unsubscribe (port_subscribe.handle, port, protocol, features)
                   : 1;
    }

private:
    LV2UI_Port_Subscribe port_subscribe { nullptr, nullptr, nullptr };
};

} // namespace lvtk
