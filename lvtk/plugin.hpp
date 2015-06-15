/*
    plugin.hpp - Support file for writing LV2 plugins in C++
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    Modified by Dave Robillard, 2008
    Modified by Michael Fisher, 2012

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA
*/

#ifndef LVTK_PLUGIN_HPP
#define LVTK_PLUGIN_HPP

#include <iostream>
#include <cstdarg>
#include <cstring>
#include <string>
#include <vector>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

#include <lvtk/feature.hpp>
#include <lvtk/ext/common.h>
#include <lvtk/ext/bufsize.hpp>
#include <lvtk/ext/resize_port.hpp>
#include <lvtk/ext/state.hpp>
#include <lvtk/ext/worker.hpp>

#include "private/debug.hpp"

/** @mainpage LV2 Toolkit

    @section intro Introduction
    These documents describe some C++ classes that may be of use if you want
    to write LV2 plugins in C++. They implement most of the boilerplate code
    so you only have to write the parts that matter, and hide the low-level
    C API.

    <b>Libraries</b>
    The classes are separated into two libraries. One, called liblvtk-plugin,
    contains the classes Plugin and Synth, defined in the files
    <lvtk/plugin.hpp> and <lvtk/synth.hpp>. They are base classes that you
    can inherit in order to create new LV2 plugins. The other library, called 
    liblvtk-ui, contains the class UI, defined in the file <lvtk/ui.hpp>
    which you can use in a similar way to create new LV2 plugin UIs.

    <b>Packages</b>
    There are a few included pkg-config packages to choose from when writing
    a plugins and UIs.  Below is a list of all packages included and what they
    are for.

    - <b>lvtk-plugin</b> - Use when writing an LV2 plugin
    - <b>lvtk-ui</b> - Use if NOT using a @ref toolkitmixins "Toolkit Mixin"
    - <b>lvtk-gtkui</b> - Use if using the GtkUI @ref toolkitmixins "Toolkit Mixin"

    In summary, there is one package for a plugin and one for each
    type of supported UI. Each UI package includes the same static library plus
    dependencies of its cooresponding toolkit.

    <b>Mixins</b>
    For both the plugin and the GUI class there are other helper classes called
    @ref pluginmixins "mixins" that you can use to add extra functionality to 
    your plugins, for example support for LV2 extensions.

    <b>Versioning</b>
    These libraries are only available as static libraries (and most of the
    code is template classes in header files), thus ABI stability is not an 
    issue. The API will be stable between major version bumps, at which the
    pkg-config name would change to prevent plugins from building against an
    incompatible version.

    @author Lars Luthman <lars.luthman@gmail.com>
    @author Michael Fisher <mfisher31@gmail.com>
 */



namespace lvtk {

    using std::vector;

    /** @internal
        A thin wrapper around std::vector<LV2_Descriptor> that frees the URI
        members of the descriptors. */
    class DescList : public vector<LV2_Descriptor> {
    public:
        ~DescList();
    };


    /** @internal
        This returns a list of all registered plugins. It is only used
        internally. */
    DescList& get_lv2_descriptors();


    /** This is a template base class for LV2 plugins. It has default
        implementations for all functions, so you only have to implement the
        functions that you need (for example run()). All subclasses must have
        a constructor whose signature matches the one in the example code below,
        otherwise it will not work with the register_class() function. The
        host will use this @c double parameter to pass the sample rate that the
        plugin should run at when it creates a new instance of the plugin.

        This is a template so that simulated dynamic binding can be used for
        the callbacks. This is not all that useful for simple plugins but it may
        come in handy when using @ref pluginmixins "mixins" and it doesn't add
        any additional vtable lookup and function call costs, like real dynamic
        binding would.
@code
      #include <cstring>
      #include <lvtk/plugin.hpp>

      class TestLV2 : public Plugin<TestLV2>
      {
      public:

        TestLV2(double) : plugin<TestLV2>(2) { }

        void run(uint32_t sample_count)
        {
          std::memcpy(p(1), p(0), sample_count * sizeof(float));
        }

      };

      static unsigned _ = TestLV2::register_class("http://lvtoolkit.org/plugins/TestLV2");
@endcode

        If the above code is compiled and linked with @c -ldaps-plugin0 into a
        shared module, it could form the shared object part of a fully
        functional (but not very useful) LV2 plugin with one audio input port
        and one audio output port that just copies the input to the output.

        You can extend your plugin classes, for example adding support for
        LV2 extensions, by passing @ref pluginmixins "mixin classes" as template
        parameters to plugin (second template parameter and onwards).

        If you want to write a synth plugin you should probably inherit the
        Synth class instead of this one.
        @headerfile lvtk/plugin.hpp
     */

    template <class Derived,
              class Ext1 = end, class Ext2 = end, class Ext3 = end,
              class Ext4 = end, class Ext5 = end, class Ext6 = end,
              class Ext7 = end, class Ext8 = end, class Ext9 = end>
    class Plugin : public MixinTree<Derived, Ext1, Ext2, Ext3,
                                             Ext4, Ext5, Ext6,
                                             Ext7, Ext8, Ext9>
    {
    public:

        /** This constructor is needed to initialise the port vector with the
            correct number of ports, and to check if all the required features
            are provided. This must be called as the first item in the
            initialiser list for your plugin class.
            @param ports The number of ports in this plugin.
         */
        Plugin (uint32_t ports)
            : m_ports(ports, 0), m_ok(true)
        {
            m_features = s_features;
            m_bundle_path = s_bundle_path;
            s_features = 0;
            s_bundle_path = 0;

            if (m_features)
            {
                FeatureHandlerMap hmap;
                Derived::map_feature_handlers (hmap);

                for (const Feature* const* iter = m_features; *iter != 0; ++iter)
                {
                    FeatureHandlerMap::iterator miter;
                    miter = hmap.find((*iter)->URI);

                    if (miter != hmap.end())
                    {
                        miter->second (static_cast<Derived*>(this), (*iter)->data);
                    }
                }
            }
        }


        /** Connects the ports. You shouldn't have to override this, just use
            p() to access the port buffers.

            If you do override this function, remember that if you want your plugin
            to be realtime safe this function may not block, allocate memory or
            otherwise take a long time to return.
            @param port The index of the port to connect.
            @param data_location The buffer to connect it to.
         */
        void connect_port(uint32_t port, void* data_location) {
            m_ports[port] = data_location;
        }

        /** Override this function if you need to do anything on activation.
            This is always called before the host starts using the run() function.
            You should reset your plugin to it's initial state here. */
        void activate() { }

        /** This is the process callback which should fill all output port buffers.
            You most likely want to override it - the default implementation does
            nothing.

            Remember that if you want your plugin to be realtime safe, this function
            may not block, allocate memory or take more than O(sample_count) time
            to execute.
            @param sample_count The number of audio frames to process/generate in
            this call.
         */
        void run(uint32_t sample_count) { }

        /** Override this function if you need to do anything on deactivation.
            The host calls this when it does not plan to make any more calls to
            run() (unless it calls activate() again).
         */
        void deactivate() { }

        /** Use this function to register your plugin class so that the host
            can find it. You need to do this when the shared library is loaded
            by the host. One portable way of doing that is to put the function
            call in the initialiser for a global variable, like this:

	@code
unsigned _ =  MypluginClass::register_class("http://my.plugin.class");
        @endcode

            The return value is not important, it's just there so you can use that
            trick.
         */
        static unsigned
        register_class(const char* uri)
        {
            LV2_Descriptor desc;
            std::memset(&desc, 0, sizeof(LV2_Descriptor));
            desc.URI = strdup (uri);
            desc.instantiate = &Derived::_create_plugin_instance;
            desc.connect_port = &Derived::_connect_port;
            desc.activate = &Derived::_activate;
            desc.run = &Derived::_run;
            desc.deactivate = &Derived::_deactivate;
            desc.cleanup = &Derived::_delete_plugin_instance;
            desc.extension_data = &Derived::extension_data;
            get_lv2_descriptors().push_back (desc);

            return get_lv2_descriptors().size() - 1;
        }

        /** @internal
            This is called by the plugin instantiation wrapper after the plugin
            object has been created. If it returns false the object will be
            discarded and NULL will be returned to the host.

            This function computes the AND combination of the results of check_ok()
            calls to any inherited @ref pluginmixins "mixins".
         */
        bool check_ok() {
            return m_ok && MixinTree<Derived,
                    Ext1, Ext2, Ext3, Ext4, Ext5,
                    Ext6, Ext7, Ext8, Ext9>::check_ok();
        }

    protected:

        /** Use this function to access and cast port buffers, for example
            like this:

            @code
LV2_Atom_Sequence* midi = p<LV2_Atom_Sequence>(midi_port);
            @endcode

            If you want to access a port buffer as a pointer-to-float (i.e. an audio
            or control port) you can use the non-template version instead.
            @param port The index of the port whose buffer you want to access.
         */
        template <typename T> T*&
        p(uint32_t port)
        {
            return reinterpret_cast<T*&>(m_ports[port]);
        }

        /** Use this function to access data buffers for control or audio ports.
	    @param port The index of the port whose buffer you want to access.
         */
        float*&
        p(uint32_t port) {
            return reinterpret_cast<float*&>(m_ports[port]);
        }

        /** Returns the filesystem path to the bundle that contains this plugin.
	    This may only be called after the plugin constructor is done executing.
         */
        const char*
        bundle_path() const
        {
            return m_bundle_path;
        }

        /** Sets the OK state of the plugin. If it's @c true (which is the default)
            the plugin has been instantiated OK, if @c false it has not and the
            host will discard it. You can call this in the constructor for your
            plugin class if you need to check some condition that isn't taken care
            of by a @ref pluginmixins "mixin".
	    @param ok True if the plugin instance is OK and can be used, false if
            it should be discarded.
         */
        void
        set_ok(bool ok)
        {
            m_ok = ok;
        }

        /** @internal
            This vector contains pointers to all port buffers. You don't need to
            access it directly, use the p() function instead. */
        std::vector<void*> m_ports;

    private:

        /** @internal
	    Wrapper function for connect_port().
         */
        static void _connect_port(LV2_Handle instance, uint32_t port,
                void* data_location) {
            reinterpret_cast<Derived*>(instance)->connect_port(port, data_location);
        }

        /** @internal
	    Wrapper function for activate().
         */
        static void _activate(LV2_Handle instance) {
            reinterpret_cast<Derived*>(instance)->activate();
        }

        /** @internal
	    Wrapper function for run().
         */
        static void _run(LV2_Handle instance, uint32_t sample_count) {
            reinterpret_cast<Derived*>(instance)->run(sample_count);
        }

        /** @internal
	    Wrapper function for deactivate().
         */
        static void _deactivate(LV2_Handle instance) {
            reinterpret_cast<Derived*>(instance)->deactivate();
        }

        /** @internal
            This function creates an instance of a plugin. It is used as
            the instantiate() callback in the LV2 descriptor. You should not use
            it directly. */
        static LV2_Handle _create_plugin_instance(const LV2_Descriptor* descriptor,
                                                  double sample_rate,
                                                  const char* bundle_path,
                                                  const LV2_Feature* const*
                                                  features)
        {
            // copy some data to static variables so the subclasses don't have to
            // bother with it
            s_features = features;
            s_bundle_path = bundle_path;

            if (LVTK_DEBUG)
            {
                std::stringstream ss;
                ss << "[plugin] Instantiating plugin...\n"
                   << "  Bundle path: " << bundle_path <<"\n"
                   << "  features: \n";

                FeatureIter feats (features);
                while (const Feature* feature = feats.next())
                    ss << "    " << feature->URI << "\n";

                ss << "  Creating plugin object...\n";

                std::clog << ss.str();
            }

            Derived* t = new Derived (sample_rate);

            if (LVTK_DEBUG) { std::clog<<"  Validating...\n"; }

            if (t->check_ok()) {
                if (LVTK_DEBUG)
                    std::clog<<"  Done!"<<std::endl;
                return reinterpret_cast<LV2_Handle>(t);
            }

            if (LVTK_DEBUG) {
                std::clog<<"  Failed!\n"
                        <<"  Deleting object."<<std::endl;
            }

            delete t;
            return 0;
        }

        /** @internal
            This function destroys an instance of a plugin. It is used as the
            cleanup() callback in the LV2 descriptor. You should not use it
            directly. */
        static void _delete_plugin_instance(LV2_Handle instance) {
            delete reinterpret_cast<Derived*> (instance);
        }


    private:

        /** @internal
            The feature array passed to this plugin instance. May not be valid
            after the constructor has returned.
         */
        Feature const* const* m_features;

        /** @internal
            The bundle path passed to this plugin instance. May not be valid
            after the constructor has returned.
         */
        char const* m_bundle_path;

        /** @internal
            Used to pass the feature array to the plugin without having to pass
            it through the constructor of the plugin class.
         */
        static Feature const* const* s_features;

        /** @internal
            Used to pass the bundle path to the plugin without having to pass
            it through the constructor of the plugin class.
         */
        static char const* s_bundle_path;

        /** @internal
            Local OK flag. Initialised to @c true, but the plugin class can set
            this to @c false using set_ok() in its constructor if the plugin
            instance for some reason should not be used.
         */
        bool m_ok;

    };


    // The static variables need to be initialised.
    template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
    class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
    Feature const* const*
    Plugin<Derived, Ext1, Ext2, Ext3, Ext4,
    Ext5, Ext6, Ext7, Ext8, Ext9>::s_features = 0;

    template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
    class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
    char const*
    Plugin<Derived, Ext1, Ext2, Ext3, Ext4,
    Ext5, Ext6, Ext7, Ext8, Ext9>::s_bundle_path = 0;


    /** @defgroup pluginmixins Plugin Mixins

        These template classes implement extra functionality that you may
        want to have in your plugin class, usually features. You add them
        to your class by passing them as template parameters to plugin
        when inheriting it. The internal structs of the mixin template classes,
        named @c I, will then be inherited by your plugin class, so that any
        public and protected members they have will be available to your
        plugin as if they were declared in your plugin class.

        They are done as separate template classes so they won't add to the
        code size of your plugin if you don't need them.

        There are also @ref guimixins "UI Mixins" that you can use in the same
        way with GUI.
    */

    /** @example workhorse.cpp
        @example workhorse_ui.cpp
        @example silence.cpp
        @example silence_ui.cpp
        @example beep.cpp
        @example beep_ui.cpp */

} /* namespace lvtk */


#endif /* LVTK_PLUGIN_HPP */
