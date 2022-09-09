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

#include "lvtk/ext/extension.hpp"

#include <lv2/ui/ui.h>

namespace lvtk {
/** Port index function. LV2_UI__portMap wrapper
    <http://lv2plug.in/ns/extensions/ui#portMap>
    @headerfile lvtk/ext/ui/port_map.hpp
    @ingroup ui
*/
struct PortIndex : FeatureData<LV2UI_Port_Map> {
    PortIndex() : FeatureData (LV2_UI__portMap) {}
    /** Call the port index function */
    inline uint32_t operator() (const std::string& symbol) const {
        return (data != nullptr) ? data->port_index (data->handle, symbol.c_str())
                                 : LV2UI_INVALID_PORT_INDEX;
    }
};

/** Support for UI Port Map
    @ingroup ui
    @headerfile lvtk/ext/ui/port_map.hpp
*/
template <class I>
struct PortMap : NullExtension {
    /** @private */
    PortMap (const FeatureList& features) {
        for (const auto& f : features)
            if (port_index.set (f))
                break;
    }

protected:
    /** Returns a port index for a symbol, or LV2UI_INVALID_PORT_INDEX
        if not found. It is a function object, so....
        @code
        // ...

        const auto index = port_index ("port_symbol");

        // ...
        @endcode
     */
    PortIndex port_index;
};

} // namespace lvtk
