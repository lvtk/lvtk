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
#include <lvtk/ext/atom.hpp>

namespace lvtk {

template<class S, template<class> class... E>
class Instance : public E<S>...
{
protected:
    Instance () = default;

public:
    Instance (double sample_rate, const String& bundle_path, const FeatureList& features)
        : E<S> (features)...
    {
        LV2_URID_Map* p_map = nullptr;
        for (const auto& f : features)
        {
            if (strcmp (f.URI, LV2_URID__map) == 0) {
                map.set_feature (f);
                if (auto* const m = map.c_obj())
                    forge.init (m);
            }
        }
    }
    
    virtual ~Instance() = default;

    void activate() {}
    void connect_port (uint32_t, void*) {};
    void run (uint32_t) {}
    void deactivate() {}
    void cleanup() { }

protected:
    Map map;
    AtomForge   forge;

private:
    double m_sample_rate;
    String m_bundle_path;

    friend class Plugin<S>; //<< so this can be private
    static void map_extension_data (ExtensionMap& em) {
        using pack_context = std::vector<int>;
        pack_context { (E<S>::map_extension_data (em) , 0)... };
    }
};

}

#include <lvtk/interface/interface.hpp>
#include <lvtk/interface/bufsize.hpp>
#include <lvtk/interface/log.hpp>
#include <lvtk/interface/state.hpp>
#include <lvtk/interface/worker.hpp>
