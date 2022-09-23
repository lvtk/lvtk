// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/options.hpp"
#include "lvtk/ext/urid.hpp"
#include "lvtk/optional.hpp"

#include <lv2/buf-size/buf-size.h>

#ifndef LV2_BUF_SIZE__nominalBlockLength
#    define LV2_BUF_SIZE__nominalBlockLength LV2_BUF_SIZE_PREFIX "nominalBlockLength"
#endif

namespace lvtk {
/** Description of buffer information.

    Used by the @ref BufSize extension to automatically scan for buffer details
    during instantiation. @see @ref BufSize

    @headerfile lvtk/ext/bufsize.hpp
    @ingroup lvtk
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
    void apply_options (LV2_URID_Map* map, const Option* options) {
        uint32_t minkey = map->map (map->handle, LV2_BUF_SIZE__minBlockLength);
        uint32_t maxkey = map->map (map->handle, LV2_BUF_SIZE__maxBlockLength);
        uint32_t nomkey = map->map (map->handle, LV2_BUF_SIZE__nominalBlockLength);
        uint32_t seqkey = map->map (map->handle, LV2_BUF_SIZE__sequenceSize);

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

    /** Apply options with two LV2_Feature pointers. */
    void apply_options (const LV2_Feature* const map, const LV2_Feature* const options) {
        apply_options ((LV2_URID_Map*) map->data, (const Option*) options->data);
    }
};

/** LV2 Buf Size Extension
    
    @tparam Mod your Plugin type
    @headerfile lvtk/ext/bufsize.hpp
    @ingroup ext 
 */
template <class Mod>
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

} // namespace lvtk
