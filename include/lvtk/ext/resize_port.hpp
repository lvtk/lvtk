// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/extension.hpp"

#include <lv2/resize-port/resize-port.h>

namespace lvtk {
/** Resize port status codes
    @ingroup alias
    @headerfile lvtk/ext/resize_port.hpp
*/
using ResizePortStatus = LV2_Resize_Port_Status;

/** Resize port wrapper
    @ingroup utility
    @headerfile lvtk/ext/resize_port.hpp
 */
struct PortResizer final : FeatureData<LV2_Resize_Port_Resize> {
    PortResizer() : FeatureData (LV2_RESIZE_PORT__resize) {}

    /** Resize a port buffer to at least @a size bytes.

        This function MAY return an error, in which case the port buffer
        was not resized and the port is still connected to the same location.
        Plugins MUST gracefully handle this situation.

        This function is in the audio threading class.

        The host MUST preserve the contents of the port buffer when resizing.

        Plugins MAY resize a port many times in a single run callback.  Hosts
        SHOULD make this as inexpensive as possible.
     */
    inline ResizePortStatus resize (uint32_t index, size_t size) const {
        return (nullptr == data) ? LV2_RESIZE_PORT_ERR_UNKNOWN
                                 : data->resize (data->data, index, size);
    }
   
    inline ResizePortStatus operator() (uint32_t index, size_t size) const {
        return resize (index, size);
    }
};

/** Adds Port Resize support
    @ingroup ext
    @headerfile lvtk/ext/resize_port.hpp
*/
template <class I>
struct ResizePort : NullExtension {
    /** @private */
    ResizePort (const FeatureList& features) {
        for (const auto& f : features) {
            if (_resizer.set (f))
                break;
        }
    }

    /** Resize a port buffer to at least @a size bytes.

        This function MAY return an error, in which case the port buffer
        was not resized and the port is still connected to the same location.
        Plugins MUST gracefully handle this situation.

        This function is in the audio threading class.

        The host MUST preserve the contents of the port buffer when resizing.

        Plugins MAY resize a port many times in a single run callback.  Hosts
        SHOULD make this as inexpensive as possible.
     */
    inline ResizePortStatus resize_port (uint32_t index, size_t size) const {
        _resizer.resize (index, size);
    }

private:
    /** Call this to request the host resize a port */
    PortResizer _resizer;
};

} // namespace lvtk
