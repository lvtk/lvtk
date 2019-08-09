
#pragma once

#include <lvtk/ext/bufsize.hpp>
#include <lvtk/interface/interface.hpp>

namespace lvtk {

template<class I>
struct BufSize : NullInterface
{
    BufSize (const FeatureList& features)
    {
        memset (&m_details, 0, sizeof (BufferDetails));
        
        Map map; OptionsArray options;
        for (const auto& f : features)
        {
            m_details.update_with (f);
            map.set_feature (f);
            options.set_feature (f);
        }

        if (map.c_obj() != nullptr && options.c_obj() != nullptr)
            m_details.apply_options (map, options.c_obj());
    }

    const BufferDetails& buffer_details() const { return m_details; }

private:
    BufferDetails m_details;
};

}
