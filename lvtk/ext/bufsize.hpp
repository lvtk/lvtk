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

#include <lvtk/buffer_details.hpp>
#include <lvtk/ext/extension.hpp>

namespace lvtk {

/** BufSize Handler
    
    Scans for buffer information provided by the host. @see buffer_details().
    To use it, add BufSize to the template parameters of your Instance

    @ingroup interfaces
 */
template<class I>
struct BufSize : NullExtension
{
    /** @private */
    BufSize (const FeatureList& features)
    {
        memset (&m_details, 0, sizeof (BufferDetails));
        
        Map map; HostOptions options;
        for (const auto& f : features)
        {
            m_details.update_with (f);
            map.set_feature (f);
            options.set_feature (f);
        }

        if (map.c_obj() != nullptr && options.get_options() != nullptr)
            m_details.apply_options (map, options.get_options());
    }

    /** Get the buffer details
        
        If the required features and options were available when instantiated,
        This will be populated with host buffer information
     
        @returns The host provided buffer details
     */
    const BufferDetails& buffer_details() const { return m_details; }

private:
    BufferDetails m_details;
};

}
