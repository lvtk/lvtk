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

#include <lv2/ui/ui.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {
/** Support for UI Port Subscribe
    @ingroup ui
    @headerfile lvtk/ext/ui/port_subscribe.hpp
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
