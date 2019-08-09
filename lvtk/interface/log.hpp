
#pragma once

#include <lvtk/interface/interface.hpp>
#include <lvtk/ext/log.hpp>

namespace lvtk {

/** Adds a Logger to your instance */
template<class I> 
struct Log : NullInterface
{
    Log (const FeatureList& features) { 
        for (const auto& f : features)
            if (strcmp (f.URI, LV2_LOG__log) == 0)
                { log.set_feature (f); break; }
    }

protected:
    Logger log;
};

}
