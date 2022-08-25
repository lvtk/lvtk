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

/** @defgroup bufsize Buf Size 
    Access to buffer information

    The @ref BufSize extension scans for buffer information provided by the 
    host, and populates a @ref BufferDetails accordingly. To use it, add 
    BufSize to the template parameters of your Plugin and call buffer_details() 
    for buffer info

    @code
    class MyPlug : public lvtk::Plugin<MyPlug, lvtk::BufSize> {
    public:
        MyPlug (lvtk::Args& args) : lvtk::Plugin (args) {
            const auto& details = buffer_details();
            
            // setup plugin using details. members are optional
            // and only set if provided by the host. e.g....
            
            if (details.min && details.max) {
                // got min/max do something with them
            } else {
                // wasn't set...
            }
        }
    };
    @endcode
*/

#pragma once

#include <lv2/buf-size/buf-size.h>
#include <lvtk/ext/options.hpp>
#include <lvtk/ext/urid.hpp>
#include <lvtk/optional.hpp>

#ifndef LV2_BUF_SIZE__nominalBlockLength
    #define LV2_BUF_SIZE__nominalBlockLength LV2_BUF_SIZE_PREFIX "nominalBlockLength"
#endif

namespace lvtk {
/* @{ */
/** Description of buffer information.
    
    Used by the @ref BufSize extension to automatically scan for buffer details
    during instantiation. @see @ref BufSize

    @headerfile lvtk/ext/bufsize.hpp
 */
struct BufferDetails final {
    Optional<uint32_t> min;           /**< <http://lv2plug.in/ns/ext/buf-size#minBlockLength> */
    Optional<uint32_t> max;           /**< <http://lv2plug.in/ns/ext/buf-size#maxBlockLength> */
    Optional<uint32_t> nominal;       /**< <http://lv2plug.in/ns/ext/buf-size#nominalBlockLength> */
    Optional<uint32_t> sequence_size; /**< <http://lv2plug.in/ns/ext/buf-size#sequenceSize> */

    /** Update with Options. Updates `min`, `max`, and `nominal`, and 
        `sequence_size` if found in the Option array

        @param map      Map for getting required LV2_URIDs
        @param options  The Options array to scan. MUST be valid with a
                        zeroed option at the end.
     */
    void apply_options (Map& map, const Option* options) {
        uint32_t minkey = map (LV2_BUF_SIZE__minBlockLength);
        uint32_t maxkey = map (LV2_BUF_SIZE__maxBlockLength);
        uint32_t nomkey = map (LV2_BUF_SIZE__nominalBlockLength);
        uint32_t seqkey = map (LV2_BUF_SIZE__sequenceSize);

        for (uint32_t i = 0;; ++i) {
            const auto& opt = options[i];
            if (opt.key == 0 || opt.value == nullptr)
                break;

            if (minkey == opt.key)
                min = *(uint32_t*) opt.value;
            else if (maxkey == opt.key)
                max = *(uint32_t*) opt.value;
            else if (seqkey == opt.key)
                sequence_size = *(uint32_t*) opt.value;
            else if (nomkey == opt.key)
                nominal = *(uint32_t*) opt.value;
        }
    }
};

/** LV2 Buf Size Extension
    @headerfile lvtk/ext/bufsize.hpp
 */
template <class I>
struct BufSize : NullExtension {
    /** @private */
    BufSize (const FeatureList& features) {
        Map map;
        OptionsData options;
        for (const auto& f : features) {
            if (! map)
                map.set (f);
            if (! options)
                options.set (f);
            if (map && options) {
                details.apply_options (map, options);
                break;
            }
        }
    }

    /** Get the buffer details
        
        If the required features and options were available when instantiated,
        This will be populated with host buffer information
     
        @returns The host provided buffer details
     */
    const BufferDetails& buffer_details() const { return details; }

private:
    BufferDetails details;
};
/* @} */
} // namespace lvtk
