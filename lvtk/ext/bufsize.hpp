/*
    bufsize.hpp - Support file for writing LV2 plugins in C++

    @@ISC@@
*/

/** @headerfile lvtk/ext/bufsize.hpp */

#pragma once

#include <lvtk/feature.hpp>
#include <lv2/buf-size/buf-size.h>
// #include <lvtk/ext/options.hpp>
#include <lvtk/ext/urid.hpp>

namespace lvtk {

struct BufferInfo
{
    uint32_t min;
    uint32_t max;
    uint32_t sequence_size;
    bool bounded;
    bool fixed;
    bool power_of_two;
};

struct BufSize
{
    BufSize() {
        memset (&m_info, 0, sizeof (BufferInfo));
    }

    /** @skip */
    void set_feature (const Feature& feature)
    {
        if (strcmp (LV2_BUF_SIZE__boundedBlockLength, feature.URI) ==0) {
            m_info.bounded = true;
        } else if (strcmp (LV2_BUF_SIZE__powerOf2BlockLength, feature.URI) ==0) {
            m_info.power_of_two = true;
        } else if (strcmp (LV2_BUF_SIZE__fixedBlockLength, feature.URI) ==0) {
            m_info.fixed = true;
        }
    }

    const BufferInfo& get_buffer_info() const
    {
        if (! m_checked)
        {
           #if 0
            uint32_t min      (map (LV2_BUF_SIZE__minBlockLength));
            uint32_t max      (map (LV2_BUF_SIZE__maxBlockLength));
            uint32_t seq_size (map (LV2_BUF_SIZE__sequenceSize));

            OptionsIter iter (plugin->get_supplied_options());
            while (const Option* opt = iter.next())
            {
                if (min == opt->key)
                    m_info.min = *(uint32_t*) opt->value;
                if (max == opt->key)
                    m_info.max = *(uint32_t*) opt->value;
                if (seq_size == opt->key)
                    m_info.sequence_size = *(uint32_t*) opt->value;
            }
           #endif
            m_checked = true;
        }

        return m_info;
    }

private:
    mutable bool m_checked = false;
    mutable BufferInfo m_info;
};

}
