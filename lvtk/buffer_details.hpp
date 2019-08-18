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

#include <lvtk/host_options.hpp>
#include <lvtk/map.hpp>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>

namespace lvtk {

/** Description of buffer details
    
    Used by the BufSize interface to automatically scan for buffer information
    during instantiation. @see BufSize

    @headerfile lvtk/buffer_details.hpp
 */
struct BufferDetails final {
    uint32_t min            = 0;
    uint32_t max            = 0;
    uint32_t sequence_size  = 0;
    bool bounded            = false;
    bool fixed              = false;
    bool power_of_two       = false;

    /** Update with a Feature
     
        Sets bufsize:boundedBlockLength, bufsize:powerOf2BlockLength, or 
        bufsize:fixedBlockLength if the Feature's URI matches.
     */
    void update_with (const Feature& feature)
    {
        if (strcmp (LV2_BUF_SIZE__boundedBlockLength, feature.URI) ==0) {
            bounded = true;
        } else if (strcmp (LV2_BUF_SIZE__powerOf2BlockLength, feature.URI) ==0) {
            power_of_two = true;
        } else if (strcmp (LV2_BUF_SIZE__fixedBlockLength, feature.URI) ==0) {
            fixed = true;
        }
    }

    /** Update with options
        
        Updates bufsize:minBlockLength, bufsize:maxBlockLength, and
        bufsize:sequenceSize if found in the Options array

        @param map      Map for getting required LV2_URIDs
        @param options  The Options array to scan
     */
    void apply_options (Map& map, const Option* options)
    {
        uint32_t min      (map (LV2_BUF_SIZE__minBlockLength));
        uint32_t max      (map (LV2_BUF_SIZE__maxBlockLength));
        uint32_t seq_size (map (LV2_BUF_SIZE__sequenceSize));

        OptionsIterator iter (options);
        while (const Option* opt = iter.next())
        {
            if (min == opt->key)
                min = *(uint32_t*) opt->value;
            if (max == opt->key)
                max = *(uint32_t*) opt->value;
            if (seq_size == opt->key)
                sequence_size = *(uint32_t*) opt->value;
        }
    }
};

}
