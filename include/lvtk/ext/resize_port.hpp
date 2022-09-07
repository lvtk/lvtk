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

/** @defgroup resize_port Resize Port
    Resizing plugin ports
*/

#pragma once

#include <lv2/resize-port/resize-port.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {
/** Resize port status codes
    @ingroup resize_port
    @headerfile lvtk/ext/resize_port.hpp
*/
using ResizePortStatus = LV2_Resize_Port_Status;

/** Resize port wrapper
    @ingroup resize_port
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
    inline ResizePortStatus operator() (uint32_t index, size_t size) const {
        return (nullptr == data) ? LV2_RESIZE_PORT_ERR_UNKNOWN
                                 : data->resize (data->data, index, size);
    }
};

/** Adds Port Resize support
    @ingroup resize_port
    @headerfile lvtk/ext/resize_port.hpp
*/
template <class I>
struct ResizePort : NullExtension {
    /** @private */
    ResizePort (const FeatureList& features) {
        for (const auto& f : features) {
            if (resize_port.set (f))
                break;
        }
    }

protected:
    /** Call this to request the host resize a port */
    PortResizer resize_port;
};

} // namespace lvtk
