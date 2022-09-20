// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

/** @defgroup plugin Plugin

    Writing an LV2 Plugin

    Plugin descriptors are registered on the stack at the global scope. First
    make a sublcass of @ref lvtk::Plugin, then register it with @ref lvtk::Descriptor.

    <h3>Example</h3>
    @include "volume.cpp"
    @{
*/

#pragma once

#include "lvtk/lvtk.hpp"

#include <map>
#include <memory>

namespace lvtk {
/** A list of LV2_Descriptors. Used internally to manage registered plugins */
using PluginDescriptors = DescriptorList<LV2_Descriptor>;

/** @fn Access to registered plugin descriptors
    @returns Plugin descriptor list
 */
inline PluginDescriptors& descriptors() {
    static PluginDescriptors s_descriptors;
    return s_descriptors;
}

/** Registers a plugin instance of type @em `P`

    Create a static one of these to register your plugin instance type.

    @code
        static lvtk::Descriptor<MyPlugin> my_plugin (
            MY_PLUGIN_URI,           //< MyPlugin's URI String
            {
                LV2_URID__map,       //< List of required host features
                LV2_WORKER__schedule
            }
        );
    @endcode

    @headerfile lvtk/plugin.hpp
    @see @ref Plugin
 */
template <class P>
class Descriptor final {
public:
    /** Plugin registration with required host features

        @param plugin_uri   The URI string of your plugin
        @param required     List of required host feature URIs. If the host fails
                            to provide any of these, instantiate will return
                            a nullptr
     */
    Descriptor (const char* plugin_uri, const std::vector<std::string>& required) {
        for (const auto& req : required)
            P::required().push_back (req);
        register_plugin (plugin_uri);
    }

    /** Plugin Registation without required host features
        @param plugin_uri   The URI string of your plugin
     */
    Descriptor (const char* plugin_uri) {
        register_plugin (plugin_uri);
    }

    ~Descriptor() = default;

private:
    inline void register_plugin (const char* uri) {
        LV2_Descriptor desc;
        desc.URI = (const char*) malloc ((1 + strlen (uri)) * sizeof (char));
        strcpy ((char*) desc.URI, uri);
        desc.instantiate    = P::_instantiate;
        desc.activate       = P::_activate;
        desc.connect_port   = P::_connect_port;
        desc.run            = P::_run;
        desc.deactivate     = P::_deactivate;
        desc.cleanup        = P::_cleanup;
        desc.extension_data = P::_extension_data;
        descriptors().push_back (desc);
        P::initialize_extensions();
    }
};

/** Arguments passed to a @ref Plugin "plugin" instance */
struct Args {
    /** @private */
    Args() : sample_rate (0.0), bundle(), features() {}
    /** @private */
    Args (double r, const std::string& b, const FeatureList& f)
        : sample_rate (r), bundle (b), features (f) {}

    double sample_rate;   /**< Sample Rate */
    std::string bundle;   /**< Bundle Path */
    FeatureList features; /**< Host provided features */
};

/** A template base class for LV2 plugin instances. Default implementations
    exist for most methods, so you only have to implement @ref connect_port()
    and @ref run().

    The signature of subclass must match the one in the example code below. The
    @c args should be used in your constructor to get "details" about instantiation.

    Since this is a template, simulated dynamic binding is used for the callbacks.
    When using @ref Extension "Extensions" no vtable lookups are invoked, like normal
    dynamic binding would.

    <h3>Copy Audio Example</h3>
    @code
        #include <lvtk/plugin.hpp>

        #include <cstring>

        using namespace lvtk;

        class CopyAudio : public Plugin<CopyAudio>
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

        // Register a descriptor for this plugin type
        static Descriptor<CopyAudio> copyaudio ("http://lvtoolkit.org/plugins/CopyAudio");

    @endcode

    If the above code is compiled into a shared module, it could form the binary
    part of a fully functional LV2 plugin with one audio input port and one audio
    output port that just copies the input to the output.

    You can extend your instance by passing @ref Extension "Extensions" as
    template parameters to Instance (second template parameter and onwards).

    @tparam S   Your super class
    @tparam E   List of Extension mixins

    @see \ref BufSize, \ref Log, \ref Options, \ref ResizePort, \ref State,
         \ref URID, \ref Worker,

    @headerfile lvtk/plugin.hpp
    @ingroup plugin
 */
template <class S, template <class> class... E>
class Plugin : public E<S>... {
protected:
    /** Default constructor not allowed */
    Plugin() = delete;

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

    /** Override this to add custom extension data without having to implement
        an @ref Extension mixin.  Also, it will be called after the the mixins,
        so if needed you can effectively override mixin extension data.
     */
    static void map_extension_data (ExtensionMap&) {}

private:
    friend class Descriptor<S>; // so this can be private

    inline static ExtensionMap& extensions() {
        static ExtensionMap s_extensions;
        return s_extensions;
    }

    inline static void initialize_extensions() {
        using pack_context = std::vector<int>;
        pack_context { (E<S>::map_extension_data (extensions()), 0)... };
        S::map_extension_data (extensions());
    }

    inline static std::vector<std::string>& required() {
        static std::vector<std::string> s_required;
        return s_required;
    }

    inline static LV2_Handle _instantiate (const LV2_Descriptor* descriptor,
                                           double sample_rate,
                                           const char* bundle_path,
                                           const LV2_Feature* const* features) {
        const Args args (sample_rate, bundle_path, features);
        auto instance = std::unique_ptr<S> (new S (args));

        for (const auto& rq : required()) {
            bool provided = false;
            for (const auto& f : args.features)
                if (f == rq) {
                    provided = true;
                    break;
                }

            if (! provided)
                return nullptr;
        }

        return static_cast<LV2_Handle> (instance.release());
    }

    inline static void _activate (LV2_Handle handle) {
        (static_cast<S*> (handle))->activate();
    }

    inline static void _connect_port (LV2_Handle handle, uint32_t port, void* data) {
        (static_cast<S*> (handle))->connect_port (port, data);
    }

    inline static void _run (LV2_Handle handle, uint32_t sample_count) {
        (static_cast<S*> (handle))->run (sample_count);
    }

    inline static void _deactivate (LV2_Handle handle) {
        (static_cast<S*> (handle))->deactivate();
    }

    inline static void _cleanup (LV2_Handle handle) {
        (static_cast<S*> (handle))->cleanup();
        delete static_cast<S*> (handle);
    }

    inline static const void* _extension_data (const char* uri) {
        auto e = extensions().find (uri);
        return e != extensions().end() ? e->second : nullptr;
    }
};


} // namespace lvtk

extern "C" {

#ifndef LVTK_NO_SYMBOL_EXPORT
/** @private */
LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index) {
    return (index < lvtk::descriptors().size())
               ? &lvtk::descriptors()[index]
               : NULL;
}
#endif
}

/* @} */
