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

/** @defgroup plugin Plugin
 
    Writing an LV2 Plugin
   
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
    @endcode

    If you are using this library in a host, include individual
    headers from the ext directory.

    Same policy for the interface directory, including anything
    there will result in a mess of compiler errors.  Interfaces
    are for implementing features on a plugin instance only
    @{
*/

#pragma once

#include <map>
#include <memory>
#include <lvtk/lvtk.hpp>
 
namespace lvtk {
/** A list of LV2_Descriptors. Used internally to manage registered plugins */
using PluginDescriptors = DescriptorList<LV2_Descriptor>;

/** @fn Access to registered plugin descriptors
    @returns Plugin descriptor list
 */
static PluginDescriptors& descriptors() {
    static PluginDescriptors s_descriptors;
    return s_descriptors;
}

/** Registers a plugin instance of type @em`I`
        
    Create a static one of these to register your plugin instance type.
    
    @code
        using MyPlugin = lvtk::Plugin<MyInstance>;
        static MyPlugin my_plugin (
            MY_PLUGIN_URI,           //< MyPlugin's URI String
            {                            
                LV2_URID__map,       //< List of required host features
                LV2_WORKER__schedule
            }
        );
    @endcode

    @headerfile lvtk/plugin.hpp
    @see @ref Instance
 */
template<class I>
class Plugin final
{
public:    
    /** Plugin registration with required host features
        
        @param plugin_uri   The URI string of your plugin
        @param required     List of required host feature URIs. If the host fails
                            to provide any of these, instantiate will return
                            a nullptr
     */
    Plugin (const char* plugin_uri, const std::vector<std::string>& required) {
        for (const auto& req : required)
            Plugin<I>::required().push_back (req);
        register_instance (plugin_uri);
    }

    /** Plugin Registation without required host features        
        @param plugin_uri   The URI string of your plugin
     */
    Plugin (const char* plugin_uri) {
        register_instance (plugin_uri);
    }

    ~Plugin() = default;

    /** Helper to register plugin extension data but not have to implement
        the a mixin interface.

        @param uri      The uri of your feature.
        @param data     Pointer to static extension data.
     */
    static void register_extension (const std::string& uri, const void* data) {
        extensions()[uri] = data;
    }

private:    
    inline void register_instance (const char* uri) {
        LV2_Descriptor desc;
        desc.URI            = strdup (uri);
        desc.instantiate    = _instantiate;
        desc.connect_port   = _connect_port;
        desc.activate       = _activate;
        desc.run            = _run;
        desc.deactivate     = _deactivate;
        desc.cleanup        = _cleanup;
        desc.extension_data = _extension_data;
        descriptors().push_back (desc);

        auto& extmap = extensions();
        I::map_extension_data (extmap);
    }

    inline static ExtensionMap& extensions() {
        static ExtensionMap s_extensions;  
        return s_extensions; 
    }

    inline static std::vector<std::string>& required() 
    {
        static std::vector<std::string> s_required;
        return s_required;
    }

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
    
       #if LVTK_STATIC_ARGS
        auto& args = I::args();
        Args::Cleared sr (args);
        args.sample_rate = sample_rate;
        args.bundle = bundle_path;
        for (int i = 0; features[i]; ++i)
            args.features.push_back (*features[i]);
       #else
        Args args (sample_rate, bundle_path, features);
       #endif

        auto instance = std::unique_ptr<I> (new I (args));

        for (const auto& rq : required()) {
            bool provided = false;
            for (const auto& f : args.features)
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
        auto e = extensions().find (uri);
        return e != extensions().end() ? e->second : nullptr;
    }
};
/* @} */
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
