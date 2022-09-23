// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/extension.hpp"

#include <lv2/ui/ui.h>

namespace lvtk {
/** Port index function. LV2_UI__portMap wrapper
    <http://lv2plug.in/ns/extensions/ui#portMap>
    @headerfile lvtk/ext/port_map.hpp
    @ingroup utility
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
    @ingroup ext
    @headerfile lvtk/ext/port_map.hpp
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
