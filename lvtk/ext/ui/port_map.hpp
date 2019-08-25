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

#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {

/** Support for UI Port Map
    @ingroup ui
    @headerfile lvtk/ext/ui/port_map.hpp
*/
template<class I> 
struct PortMap : NullExtension
{
    /** @private */
    PortMap (const FeatureList& features) {
        for (const auto& f : features) {
            if (f == LV2_UI__portMap) {
                port_map = *(LV2UI_Port_Map*) f.data;
                break;
            }
        }
    }

    /** Returns a port index for a symbol */
    uint32_t port_index (const std::string& symbol) const {
        return (port_map.handle != nullptr)
            ? port_map.port_index (port_map.handle, symbol.c_str())
            : LV2UI_INVALID_PORT_INDEX;
    }

private:
    LV2UI_Port_Map port_map { nullptr, nullptr };
};

}
