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

#include <lvtk/lvtk.hpp>

namespace lvtk {

/** Convenient typedef for a vector of Features. */
struct FeatureList final : public std::vector<Feature>
{
    FeatureList() = default;
    FeatureList (const FeatureList& o) {
        for (const auto& f : o)
            push_back (f);
    }

    FeatureList (const LV2_Feature *const * features) {
        for (int i = 0; features[i]; ++i) {
            push_back (*features[i]);
        }
    }

    inline void get_uris (StringArray& uris) {
        for (const auto& f : *this)
            uris.push_back (f.URI);
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
