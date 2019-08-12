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

#include <map>
#include <lvtk/ext/urid.hpp>

/** LV2 Plugin Implementation
    
    Only include this header in a file that implements an LV2 Plugin
    via a subclass of lvtK::Instance

    Plugin descriptors are registered on the stack at the global scope.
    For example...
    
    @code
        using MyPlugin = lvtk::Plugin<MyInstance>;
        
        static MyPlugin my_plugin (
            LVTK_SILENCE_URI,           //< MyPlugin's URI String
            {                            
                LV2_URID__map,          //< List of required host features
                LV2_WORKER__schedule
            }
        );
    @code

    If you are using this library in a host, include individual
    headers from the ext directory.

    Same policy for the interface directory, including anything
    there will result in a mess of compiler errors.  Interfaces
    are for implementing features on a plugin instance only
 */
 
namespace lvtk {

using PluginDescriptors = DescriptorList<LV2_Descriptor>;

/** Returns a global array of registered descriptors */
static PluginDescriptors& descriptors() {
    static PluginDescriptors s_descriptors;
    return s_descriptors;
}

/** Plugin registration class
    Create a static one of these to register your plugin instance type.
 */
template<class I>
class Plugin final
{
public:    
    /** Plugin Registation
        
        @param plugin_uri   The URI string of your plugin
        @param required     List of required host feature URIs. If the host fails
                            to provide any of these, instantiate will return
                            a nullptr
     */
    Plugin (const char* plugin_uri, const std::vector<std::string>& required = {})
    {
        LV2_Descriptor desc;
        desc.URI = strdup (plugin_uri);
        desc.instantiate    = _instantiate;
        desc.connect_port   = _connect_port;
        desc.activate       = _activate;
        desc.run            = _run;
        desc.deactivate     = _deactivate;
        desc.cleanup        = _cleanup;
        desc.extension_data = _extension_data;
        descriptors().push_back (desc);

        for (const auto& req : required) {
            s_required.push_back (req);
        }

        PluginInstance::map_extension_data (s_extensions);
    }

    ~Plugin() = default;

    /** Helper to register plugin extension data but not have to implement
        the a mixin interface.

        @param uri      The uri of your feature.
        @param data     Pointer to static extension data.
     */
    static void register_extension (const std::string& uri, const void* data) {
        s_extensions[uri] = data;
    }

private:
    using PluginInstance = I;
    static ExtensionMap s_extensions;
    static std::vector<std::string>  s_required;
    /** @internal */
    static LV2_Handle _instantiate (const struct _LV2_Descriptor * descriptor,
	                                double                         sample_rate,
	                                const char *                   bundle_path,
	                                const LV2_Feature *const *     features)
    {

       #ifndef NDEBUG
        #define debug(expr) std::clog << expr
       #else
        #define debug(expr)
       #endif

        const FeatureList host_features (features);

        auto instance = std::unique_ptr<I> (new I (sample_rate, bundle_path, host_features));

        for (const auto& rq : s_required) {
            bool provided = false;
            for (const auto& f : host_features)
                if (f == rq) { provided = true; break; }
            
            if (! provided)
                return nullptr;
        }

        return static_cast<LV2_Handle> (instance.release());
    }
    
    /** @internal */
    static void _connect_port (LV2_Handle handle, uint32_t port, void* data) {
        (static_cast<I*> (handle))->connect_port (port, data);
    }

    /** @internal */
    static void _activate (LV2_Handle handle) { 
        (static_cast<I*> (handle))->activate();
    }

    /** @internal */
    static void _run (LV2_Handle handle, uint32_t sample_count) {
        (static_cast<I*> (handle))->run (sample_count);
    }
    
    /** @internal */
    inline static void _deactivate (LV2_Handle handle) {
        (static_cast<I*> (handle))->deactivate();
    }

    /** @internal */
    inline static void _cleanup (LV2_Handle handle) {
        (static_cast<I*> (handle))->cleanup();
        delete static_cast<I*> (handle);
    }

    /** @internal */
    inline static const void* _extension_data (const char* uri) {
        auto e = s_extensions.find (uri);
        return e != s_extensions.end() ? e->second : nullptr;
    }
};

template<class I> ExtensionMap Plugin<I>::s_extensions = {};
template<class I> std::vector<std::string> Plugin<I>::s_required;

}

#include <lvtk/instance.hpp>

extern "C" {

#ifndef LVTK_NO_SYMBOL_EXPORT

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index) {
    return (index < lvtk::descriptors().size())
        ? &lvtk::descriptors()[index] : NULL;
}

#endif

}
