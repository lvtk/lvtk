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
            // setup plugin using details
        }
    };
    @endcode
*/

#pragma once

#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include <lvtk/ext/options.hpp>
#include <lvtk/ext/urid.hpp>

namespace lvtk {
/* @{ */
/** Description of buffer information.
    
    Used by the @ref BufSize extension to automatically scan for buffer details
    during instantiation. @see @ref BufSize

    @headerfile lvtk/ext/bufsize.hpp
 */
struct BufferDetails final {
    bool bounded            = false;    /**< <http://lv2plug.in/ns/ext/buf-size#boundedBlockLength> */
    bool fixed              = false;    /**< <http://lv2plug.in/ns/ext/buf-size#fixedBlockLength> */
    bool power_of_two       = false;    /**< <http://lv2plug.in/ns/ext/buf-size#powerOf2BlockLength> */
    uint32_t min            = 0;        /**< <http://lv2plug.in/ns/ext/buf-size#minBlockLength> */
    uint32_t max            = 0;        /**< <http://lv2plug.in/ns/ext/buf-size#maxBlockLength> */
    uint32_t nominal        = 0;        /**< <http://lv2plug.in/ns/ext/buf-size#nominalBlockLength> */
    uint32_t sequence_size  = 0;        /**< <http://lv2plug.in/ns/ext/buf-size#sequenceSize> */

    /** Update with a Feature. Sets `bounded`, `power_of_two`, or 
        `fixed` if the Feature's URI matches.
     */
    void update_with (const Feature& feature)
    {
        if (feature == LV2_BUF_SIZE__boundedBlockLength) {
            bounded = true;
        } else if (feature == LV2_BUF_SIZE__powerOf2BlockLength) {
            power_of_two = true;
        } else if (feature == LV2_BUF_SIZE__fixedBlockLength) {
            fixed = true;
        }
    }

    /** Update with Options. Updates `min`, `max`, and `nominal`, and 
        `sequence_size` if found in the Option array

        @param map      Map for getting required LV2_URIDs
        @param options  The Options array to scan and MUST be valid with a
                        zeroed option at the end.
     */
    void apply_options (Map& map, const Option* options)
    {
        uint32_t minlen   = map (LV2_BUF_SIZE__minBlockLength);
        uint32_t maxlen   = map (LV2_BUF_SIZE__maxBlockLength);
        uint32_t seq_size = map (LV2_BUF_SIZE__sequenceSize);
        uint32_t nomkey   = map (LV2_BUF_SIZE__nominalBlockLength);
        
        for (uint32_t i = 0;;++i)
        {
            const auto& opt = options [i];
            if (opt.key == 0 || opt.value == nullptr)
                break;

            if (minlen == opt.key)
                min = *(uint32_t*) opt.value;
            else if (maxlen == opt.key)
                max = *(uint32_t*) opt.value;
            else if (seq_size == opt.key)
                sequence_size = *(uint32_t*) opt.value;
            else if (nomkey == opt.key)
                nominal = *(uint32_t*) opt.value;
        }        
    }
};

/** LV2 Buf Size Extension
    @headerfile lvtk/ext/bufsize.hpp
 */
template<class I>
struct BufSize : NullExtension
{
    /** @private */
    BufSize (const FeatureList& features)
    {
        memset (&details, 0, sizeof(BufferDetails));
        Map map; OptionsData options;
        for (const auto& f : features)
        {
            details.update_with (f);
            map.set (f);
            options.set (f);
        }

        if (map.get() != nullptr && options.get() != nullptr)
            details.apply_options (map, options.get());
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
}
