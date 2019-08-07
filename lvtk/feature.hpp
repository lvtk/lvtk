
#pragma once

#include <lvtk/lvtk.hpp>

namespace lvtk {

/** Convenient typedef for a vector of Features. */
struct FeatureList final : public std::vector<const LV2_Feature*>
{
    FeatureList() = default;
    FeatureList (const LV2_Feature *const * features) {
        for (int i = 0; features[i]; ++i)
            push_back (features [i]);
    }

    inline void get_uris (StringArray& uris) {
        for (const auto& f : *this)
            uris.push_back (f->URI);
    }
};

class FeatureIterator final
{
public:
    FeatureIterator (const Feature* const* features)
        : m_index (0), p_feats (features) { }

    inline const Feature* next()
    {
        if (nullptr == p_feats[m_index])
            return nullptr;
        return p_feats[m_index++];
    }

private:
    uint32_t                 m_index;
    const Feature* const*    p_feats;
};

} /* namespace lvtk */
