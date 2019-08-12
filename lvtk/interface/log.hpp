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

#include <lvtk/interface/interface.hpp>
#include <lvtk/ext/log.hpp>

namespace lvtk {

/** Adds a Logger `log` to your instance */
template<class I> 
struct Log : NullInterface
{
    Log (const FeatureList& features) { 
        for (const auto& f : features) {
            int n_ok = 0;
            if (strcmp (f.URI, LV2_LOG__log) == 0)
                { log.set_feature (f); ++n_ok; }
            if (strcmp (f.URI, LV2_URID__map) == 0)
                { log.init ((LV2_URID_Map*) f.data); ++n_ok; }
            if (n_ok >= 2)
                break;
        }
    }

protected:
    Logger log;
};

}
