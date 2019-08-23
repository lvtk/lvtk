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
class Descriptor final
{
public:
    /** Plugin registration with required host features
        
        @param plugin_uri   The URI string of your plugin
        @param required     List of required host feature URIs. If the host fails
                            to provide any of these, instantiate will return
                            a nullptr
     */
    Descriptor (const char* plugin_uri, const std::vector<std::string>& required) {
        for (const auto& req : required)
            Descriptor<I>::required().push_back (req);
        register_plugin (plugin_uri);
    }

    /** Plugin Registation without required host features        
        @param plugin_uri   The URI string of your plugin
     */
    Descriptor (const char* plugin_uri) {
        register_plugin (plugin_uri);
    }

    ~Descriptor() = default;

    /** Helper to register plugin extension data but not have to implement
        the a mixin interface.

        @param uri      The uri of your feature.
        @param data     Pointer to static extension data.
     */
    void register_extension (const std::string& uri, const void* data) {
        extensions()[uri] = data;
    }

private:    
    inline void register_plugin (const char* uri) {
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
        const Args args (sample_rate, bundle_path, features);
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

/** A template base class for LV2 plugin instances. Default implementations 
    exist for most methods, so you only have to implement @ref connect_port() 
    and @ref run().
    
    The signature of subclass must matches the one in the example code below, 
    otherwise it will not work with plugin registration. The host will use the 
    @c args parameter to pass @ref Args "details" about instantiation you can 
    use in your constructor.

    This is a template so that simulated dynamic binding can be used for
    the callbacks. This is not all that useful for simple plugins but it may
    come in handy when using @ref Extension "Extensions" and it doesn't add
    any additional vtable lookup and function call costs, like real dynamic
    binding would.

    <h3>Copy Audio Example</h3>
    @code
        #include <cstring>
        #include <lvtk/plugin.hpp>

        using namespace lvtk;

        class CopyAudio : public Instance<CopyAudio>
        {
        public:
            CopyAudio (const Args& args) : Instance (args) { }

            void connect_port (uint32_t port, void* data) {
                audio[port] = data;
            }

            void run (uint32_t sample_count) {
                std::memcpy (audio[1], audio[0], sample_count * sizeof (float));
            }
        
        private:
            float* audio[2];
        };

        // Register a descriptor for this instance type
        static Plugin<CopyAudio> copyaudio ("http://lvtoolkit.org/plugins/CopyAudio");

    @endcode

    If the above code is compiled into a shared module, it could form the binary 
    part of a fully functional LV2 plugin with one audio input port and one audio 
    output port that just copies the input to the output.

    You can extend your instance by passing @ref Extension "Extensions" as 
    template parameters to Instance (second template parameter and onwards).
    
    @see \ref BufSize, \ref Log, \ref Options, \ref ResizePort, \ref State, 
         \ref URID, \ref Worker,
    
    @headerfile lvtk/instance.hpp
    @ingroup plugin
 */
template<class S, template<class> class... E>
class Plugin : public E<S>...
{
protected:
    /** Plugin with Arguments.
      
        @param args  Arguments created during instantiation.  Your subclass
                     must pass the @ref Args here
     */
    explicit Plugin (const Args& args) : E<S> (args.features)... {}

public:
    ~Plugin() = default;

    /** Override this function if you need to do anything on activation.
        This is always called before the host starts using the @ref run() 
        function. You should reset your plugin to it's initial state here. 
     */
    void activate() {}

    /** Override this to connect to your own port buffers.

        Remember that if you want your plugin to be realtime safe this function
        may not block, allocate memory or otherwise take a long time to return.

        @param port The index of the port to connect.
        @param data The buffer to connect it to.
     */
    void connect_port (uint32_t port, void* data) {}

    /** This is the process callback which should fill all output port buffers.
        You most likely want to override it - the default implementation does
        nothing.
        
        Remember that if you want your plugin to be realtime safe, this function
        may not block, allocate memory or take more than `sample_count` time
        to execute.
        
        @param sample_count The number of audio frames to process.
     */
    void run (uint32_t sample_count) {}

    /** Override this function if you need to do anything on deactivation.
        The host calls this when it does not plan to make any more calls to
        @ref run() unless it calls @ref activate() again.
     */
    void deactivate() {}

    /** Override this to handle cleanup. Is called immediately before the 
        instance is deleted.  You only need to implement this if you'd like 
        to do something special before the destructor.
     */
    void cleanup() {}

private:
    friend class Descriptor<S>; // so this can be private

    static void map_extension_data (ExtensionMap& em) {
        using pack_context = std::vector<int>;
        pack_context { (E<S>::map_extension_data (em), 0)... };
    }
};

/* @} */
}

extern "C" {

#ifndef LVTK_NO_SYMBOL_EXPORT

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index) {
    return (index < lvtk::descriptors().size())
        ? &lvtk::descriptors()[index] : NULL;
}

#endif

}
