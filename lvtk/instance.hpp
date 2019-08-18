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

/** Base class for plugin isntances
    
    Inherrit this and override it's methods to create a minimal LV2 Plugin.
    Add interface mixins to extend functionality.

    @headerfile lvtk/instance.hpp
 */
template<class S, template<class> class... E>
class Instance : public E<S>...
{
protected:
    Instance () = default;

public:
    /** Default instance constructor */
    Instance (double sample_rate, const std::string& bundle_path, const FeatureList& features)
        : E<S> (features)...
    {
        for (const auto& f : features) {
            if (strcmp (f.URI, LV2_URID__map) == 0) {
                map.set_feature (f);
                if (auto* const m = map.c_obj())
                    forge.init (m);
                break;
            }
        }
    }
    
    virtual ~Instance() = default;

    /** Override this to handle activate */
    void activate() {}

    /** Override this to handle connect_port */
    void connect_port (uint32_t, void*) {};

    /** Override this to handle run */
    void run (uint32_t) {}

    /** Override this to handle deactivate */
    void deactivate() {}

    /** Override this to handle cleanup
        Will be called immediately before the instance is deleted
     */
    void cleanup() { }

protected:
    Map         map;
    AtomForge   forge;

private:
    double m_sample_rate;
    std::string m_bundle_path;

    friend class Plugin<S>; // so this can be private
    static void map_extension_data (ExtensionMap& em) {
        using pack_context = std::vector<int>;
        pack_context { (E<S>::map_extension_data (em) , 0)... };
    }
};

}

#include <lvtk/interface/interface.hpp>
#include <lvtk/interface/bufsize.hpp>
#include <lvtk/interface/log.hpp>
#include <lvtk/interface/options.hpp>
#include <lvtk/interface/state.hpp>
#include <lvtk/interface/worker.hpp>
