/****************************************************************************
    
    lv2plugin.hpp - support file for writing LV2 plugins in C++
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    Modified by Dave Robillard, 2008

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

****************************************************************************/

#ifndef LV2PLUGIN_HPP
#define LV2PLUGIN_HPP

#include <unistd.h>
#include <stdarg.h>

#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <iostream> // XXX just for debugging

#include <lv2.h>
#include <lv2_uri_map.h>
#include <lv2-saverestore.h>
#include <lv2_event.h>
#include <lv2_contexts.h>
#include <lv2types.hpp>


/** @mainpage liblv2-plugin and liblv2-gui - API documentation
    These documents describes some C++ classes that may be of use if you want
    to write LV2 plugins in C++. They implement most of the boilerplate code
    so you only have to write the parts that matter, and hide the low-level
    C API.
    
    The classes are separated into two libraries. One, called liblv2-plugin,
    contains the classes LV2::Plugin and LV2::Synth, defined in the files 
    lv2plugin.hpp and lv2synth.hpp. They are base classes that you can inherit
    in order to create new LV2 plugins. The other library, called liblv2-gui, 
    contains the class LV2::GUI, defined in the file lv2gui.hpp, which you can
    use in a similar way to create new LV2 plugin GUIs.
    
    For both the Plugin and the GUI class there are other helper classes called
    @ref pluginmixins "mixins" that you can use to add extra functionality to 
    your plugins, for example support for LV2 extensions.

    These libraries are only available as static libraries (and most of the
    code is template classes in header files), thus ABI stability is not an 
    issue. The API will be stable between major version bumps, at which the
    pkg-config name would change to prevent plugins from building against an
    incompatible version, but if you were to modify the build system to create
    shared libraries and link against those you are on your own.
    
    This is reference documentation, if you want a more tutorial-type document
    you can have a look at <a href="http://ll-plugins.nongnu.org/lv2pftci">
    LV2 programming for the complete idiot</a>.
    
    @author Lars Luthman <lars.luthman@gmail.com>
*/


namespace LV2 {
  

  /** @internal
      A thin wrapper around std::vector<LV2_Descriptor> that frees the URI
      members of the descriptors. */
  class DescList : public std::vector<LV2_Descriptor> {
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
      #include <lv2plugin.hpp>
      
      class TestLV2 : public LV2::Plugin<TestLV2> {
      public:
        TestLV2(double) : LV2::Plugin<TestLV2>(2) { }
        void run(uint32_t sample_count) {
          std::memcpy(p(1), p(0), sample_count * sizeof(float));
        }
      };
      
      static unsigned _ = TestLV2::register_class("http://ll-plugins.sf.net/plugins/TestLV2");
      @endcode
      
      If the above code is compiled and linked with @c -llv2_plugin into a 
      shared module, it could form the shared object part of a fully 
      functional (but not very useful) LV2 plugin with one audio input port
      and one audio output port that just copies the input to the output.
      
      You can extend your plugin classes, for example adding support for
      LV2 extensions, by passing @ref pluginmixins "mixin classes" as template
      parameters to Plugin (second template parameter and onwards). 
      
      If you want to write a synth plugin you should probably inherit the 
      Synth class instead of this one.
  */
  template <class Derived, 
	    class Ext1 = End, class Ext2 = End, class Ext3 = End,
	    class Ext4 = End, class Ext5 = End, class Ext6 = End, 
	    class Ext7 = End, class Ext8 = End, class Ext9 = End>
  class Plugin : public MixinTree<Derived, 
				  Ext1, Ext2, Ext3, Ext4, Ext5, 
				  Ext6, Ext7, Ext8, Ext9> {
  public:
    
    /** This constructor is needed to initialise the port vector with the
	correct number of ports, and to check if all the required features
	are provided. This must be called as the first item in the 
	initialiser list for your plugin class.
	@param ports The number of ports in this plugin.
    */
    Plugin(uint32_t ports) 
      : m_ports(ports, 0),
	m_ok(true) {
      m_features = s_features;
      m_bundle_path = s_bundle_path;
      s_features = 0;
      s_bundle_path = 0;
      if (m_features) {
	FeatureHandlerMap hmap;
	Derived::map_feature_handlers(hmap);
	for (const Feature* const* iter = m_features; *iter != 0; ++iter) {
	  FeatureHandlerMap::iterator miter;
	  miter = hmap.find((*iter)->URI);
	  if (miter != hmap.end())
	    miter->second(static_cast<Derived*>(this), (*iter)->data);
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
unsigned _ =  MyPluginClass::register_class("http://my.plugin.class");
        @endcode
	
	The return value is not important, it's just there so you can use that
	trick.
    */
    static unsigned register_class(const std::string& uri) {
      LV2_Descriptor desc;
      std::memset(&desc, 0, sizeof(LV2_Descriptor));
      char* c_uri = new char[uri.size() + 1];
      std::memcpy(c_uri, uri.c_str(), uri.size() + 1);
      desc.URI = c_uri;
      desc.instantiate = &Derived::_create_plugin_instance;
      desc.connect_port = &Derived::_connect_port;
      desc.activate = &Derived::_activate;
      desc.run = &Derived::_run;
      desc.deactivate = &Derived::_deactivate;
      desc.cleanup = &Derived::_delete_plugin_instance;
      desc.extension_data = &Derived::extension_data;
      get_lv2_descriptors().push_back(desc);
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
LV2_Event_Buffer* midibuffer = p<LV2_Event_Buffer>(midiport_index);
        @endcode
	
	If you want to access a port buffer as a pointer-to-float (i.e. an audio
	or control port) you can use the non-template version instead.
	@param port The index of the port whose buffer you want to access.
    */
    template <typename T> T*& p(uint32_t port) {
      return reinterpret_cast<T*&>(m_ports[port]);
    }
  
    /** Use this function to access data buffers for control or audio ports.
	@param port The index of the port whose buffer you want to access.
    */
    float*& p(uint32_t port) {
      return reinterpret_cast<float*&>(m_ports[port]);
    }
    
    /** Returns the filesystem path to the bundle that contains this plugin. 
	This may only be called after the Plugin constructor is done executing.
    */
    const char* bundle_path() const {
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
    void set_ok(bool ok) {
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
					      const Feature* const* 
					      features) {

      // copy some data to static variables so the subclasses don't have to
      // bother with it
      s_features = features;
      s_bundle_path = bundle_path;

      Derived* t = new Derived(sample_rate);
      if (t->check_ok())
	return reinterpret_cast<LV2_Handle>(t);
      delete t;
      return 0;
    }
	    
    /** @internal
	This function destroys an instance of a plugin. It is used as the
	cleanup() callback in the LV2 descriptor. You should not use it
	directly. */
    static void _delete_plugin_instance(LV2_Handle instance) {
      delete reinterpret_cast<Derived*>(instance);
    }


  private:
    
    /** @internal
	The Feature array passed to this plugin instance. May not be valid
	after the constructor has returned.
    */
    LV2::Feature const* const* m_features;
    
    /** @internal
	The bundle path passed to this plugin instance. May not be valid
	after the constructor has returned.
    */
    char const* m_bundle_path;
    
    /** @internal
	Used to pass the Feature array to the plugin without having to pass
	it through the constructor of the plugin class.
    */
    static LV2::Feature const* const* s_features;

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
  LV2::Feature const* const* 
  Plugin<Derived, Ext1, Ext2, Ext3, Ext4, 
	 Ext5, Ext6, Ext7, Ext8, Ext9>::s_features = 0;
  
  template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
	   class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
  char const* 
  Plugin<Derived, Ext1, Ext2, Ext3, Ext4, 
	 Ext5, Ext6, Ext7, Ext8, Ext9>::s_bundle_path = 0;

  
  /** @defgroup pluginmixins Plugin mixins
      These template classes implement extra functionality that you may
      want to have in your plugin class, usually Features. You add them
      to your class by passing them as template parameters to LV2::Plugin
      when inheriting it. The internal structs of the mixin template classes,
      named @c I, will then be inherited by your plugin class, so that any 
      public and protected members they have will be available to your 
      plugin as if they were declared in your plugin class. 
      
      They are done as separate template classes so they won't add to the
      code size of your plugin if you don't need them. 
      
      There are also @ref guimixins "GUI mixins" that you can use in the same
      way with LV2::GUI.
  */
  

  /** The fixed buffer size extension. A host that supports this will always
      call the plugin's run() function with the same @c sample_count parameter,
      which will be equal to the value returned by I::get_buffer_size(). 

      The actual type that your plugin class will inherit when you use 
      this mixin is the internal struct template I.
      @ingroup pluginmixins
  */
  template <bool Required = true>
  struct FixedBufSize {
    
    /** This is the type that your plugin class will inherit when you use the
	FixedBufSize mixin. The public and protected members defined here
	will be available in your plugin class.
    */
    template <class Derived> struct I : Extension<Required> {
      
      /** @internal */
      I() : m_buffer_size(0) { }
      
      /** @internal */
      static void map_feature_handlers(FeatureHandlerMap& hmap) {
	hmap["http://tapas.affenbande.org/lv2/ext/fixed-buffersize"] = 
	  &I<Derived>::handle_feature;
      }
      
      /** @internal */
      static void handle_feature(void* instance, void* data) { 
	Derived* d = reinterpret_cast<Derived*>(instance);
	I<Derived>* fe = static_cast<I<Derived>*>(d);
	fe->m_buffer_size = *reinterpret_cast<uint32_t*>(data);
	fe->m_ok = true;
      }
      
    protected:
      
      /** This returns the buffer size that the host has promised to use.
	  If the host does not support this extension this function will
	  return 0. */
      uint32_t get_buffer_size() const { return m_buffer_size; }
      
      uint32_t m_buffer_size;
      
    };
    
  };
  
  /** The fixed power-of-2 buffer size extension. This works just like 
      FixedBufSize with the additional requirement that the buffer size must
      be a power of 2. 

      The actual type that your plugin class will inherit when you use 
      this mixin is the internal struct template I.
      @ingroup pluginmixins
  */
  template <bool Required = true>
  struct FixedP2BufSize {
    
    /** This is the type that your plugin class will inherit when you use the
	FixedP2BufSize mixin. The public and protected members defined here
	will be available in your plugin class.
    */
    template <class Derived> struct I : Extension<Required> {
      
      /** @internal */
      I() : m_buffer_size(0) { }
      
      /** @internal */
      static void map_feature_handlers(FeatureHandlerMap& hmap) {
	hmap["http://tapas.affenbande.org/lv2/ext/power-of-two-buffersize"] = 
	  &I<Derived>::handle_feature;
      }
      
      /** @internal */
      static void handle_feature(void* instance, void* data) { 
	Derived* d = reinterpret_cast<Derived*>(instance);
	I<Derived>* fe = static_cast<I<Derived>*>(d);
	fe->m_buffer_size = *reinterpret_cast<uint32_t*>(data);
	fe->m_ok = true;
      }
      
    protected:
      
      /** This returns the buffer size that the host has promised to use.
	  If the host does not support this extension this function will
	  return 0. */
      uint32_t get_buffer_size() const { return m_buffer_size; }
      
      uint32_t m_buffer_size;
      
    };
  
  };


  /** The save/restore extension. 
      
      The actual type that your plugin class will
      inherit when you use this mixin is the internal struct template I.
      @ingroup pluginmixins
  */
  template <bool Required = true>
  struct SaveRestore {
    
    /** This is the type that your plugin class will inherit when you use the
	SaveRestore mixin. The public and protected members defined here
	will be available in your plugin class.
    */
    template <class Derived> struct I : Extension<Required> {
      
      /** @internal */
      I() { }
      
      /** @internal */
      static void map_feature_handlers(FeatureHandlerMap& hmap) {
	hmap[LV2_SAVERESTORE_URI] = &I<Derived>::handle_feature;
      }
      
      /** @internal */
      static void handle_feature(void* instance, void* data) { 
	Derived* d = reinterpret_cast<Derived*>(instance);
	I<Derived>* fe = static_cast<I<Derived>*>(d);
	fe->m_ok = true;
      }
      
      /** @internal */
      static const void* extension_data(const char* uri) { 
	if (!std::strcmp(uri, LV2_SAVERESTORE_URI)) {
	  static LV2SR_Descriptor srdesc = { &I<Derived>::_save,
					     &I<Derived>::_restore };
	  return &srdesc;
	}
	return 0;
      }
      
      /** This function is called by the host when it wants to save the 
	  current state of the plugin. You should override it. 
	  @param directory A filesystem path to a directory where the plugin
	                   should write any files it creates while saving.
	  @param files A pointer to a NULL-terminated array of @c LV2SR_File 
	               pointers. The plugin should set @c *files to point to 
		       the first element in a dynamically allocated array of 
		       @c LV2SR_File pointers to (also dynamically allocated) 
		       @c LV2SR_File objects, listing the files to which the 
		       internal state of the plugin instance has been saved. 
		       These objects, and the array, will be freed by the host.
      */
      char* save(const char* directory, LV2SR_File*** files) { return 0; }
      
      /** This function is called by the host when it wants to restore
	  the plugin to a previous state. You should override it.
	  @param files An array of pointers to @c LV2SR_File objects, listing
	               the files from which the internal state of the plugin 
		       instance should be restored.
      */
      char* restore(const LV2SR_File** files) { return 0; }
      
    protected:
      
      /** @internal
	  Static callback wrapper. */
      static char* _save(LV2_Handle h, 
			 const char* directory, LV2SR_File*** files) {
	return reinterpret_cast<Derived*>(h)->save(directory, files);
      }
      
      /** @internal
	  Static callback wrapper. */
      static char* _restore(LV2_Handle h, const LV2SR_File** files) {
	return reinterpret_cast<Derived*>(h)->restore(files);
      }
      
    };
  };
  
  
  /** The event ref/unref function, required for plugins with event ports. 

      The actual type that your plugin class will inherit when you use 
      this mixin is the internal struct template I.
      @ingroup pluginmixins
  */
  template <bool Required = true>
  struct EventRef {
    
    /** This is the type that your plugin class will inherit when you use the
	EventRef mixin. The public and protected members defined here
	will be available in your plugin class.
    */
    template <class Derived> struct I : Extension<Required> {
      
      /** @internal */
      I() : m_callback_data(0), m_ref_func(0), m_unref_func(0) { }
      
      /** @internal */
      static void map_feature_handlers(FeatureHandlerMap& hmap) {
	hmap[LV2_EVENT_URI] = &I<Derived>::handle_feature;
      }
      
      /** @internal */
      static void handle_feature(void* instance, void* data) { 
	Derived* d = reinterpret_cast<Derived*>(instance);
	I<Derived>* fe = static_cast<I<Derived>*>(d);
        LV2_Event_Feature* ef = reinterpret_cast<LV2_Event_Feature*>(data);
        fe->m_callback_data = ef->callback_data;
        fe->m_ref_func = ef->lv2_event_ref;
        fe->m_unref_func = ef->lv2_event_unref;
        fe->m_ok = true;
      }
      
    protected:
      
      /** This should be called by the plugin for any event of type 0 if it
	  creates an additional copy of it, either by saving more than one copy
	  internally, passing more than one copy through to an output port,
	  or a combination of those. It must be called once for each additional
	  copy of the event.
	  Note that you must not call this function if you just save one copy
	  of the event, or just passes one copy through to an output port.
	  @c param event The event, as returned by @c lv2_event_get().
      */
      uint32_t event_ref(LV2_Event* event) {
	return m_ref_func(m_callback_data, event);
      }
    
      /** This should be called by the plugin for any event of type 0, unless
	  it keeps a copy of it or passes it through to an event output port.
	  @c param event The event, as returned by @c lv2_event_get().
      */
      uint32_t event_unref(LV2_Event* event) {
	return m_unref_func(m_callback_data, event);
      }
    
      LV2_Event_Callback_Data m_callback_data;
      uint32_t (*m_ref_func)(LV2_Event_Callback_Data, LV2_Event*);
      uint32_t (*m_unref_func)(LV2_Event_Callback_Data, LV2_Event*);
      
    };
    
  };


  /** @internal
      The message context extension. Experimental and unsupported. Don't use it.

      The actual type that your plugin class will inherit when you use 
      this mixin is the internal struct template I.
      @ingroup pluginmixins
  */
  template <bool Required = true>
  struct MsgContext {
    
    /** This is the type that your plugin class will inherit when you use the
	MsgContext mixin. The public and protected members defined here
	will be available in your plugin class.
    */
    template <class Derived> struct I : Extension<Required> {
      
      /** @internal */
      I() { }
      
      /** @internal */
      static void map_feature_handlers(FeatureHandlerMap& hmap) {
	hmap[LV2_CONTEXT_MESSAGE] = &I<Derived>::handle_feature;
      }
      
      /** @internal */
      static void handle_feature(void* instance, void* data) { 
	Derived* d = reinterpret_cast<Derived*>(instance);
	I<Derived>* fe = static_cast<I<Derived>*>(d);
        fe->m_ok = true;
      }
      
      /** @internal */
      static const void* extension_data(const char* uri) { 
	if (!std::strcmp(uri, LV2_CONTEXT_MESSAGE)) {
	  static LV2_Blocking_Context desc = { &I<Derived>::_blocking_run,
					       &I<Derived>::_connect_port };
	  return &desc;
	}
	return 0;
      }
      
      /** @internal
	  This is called by the host when the plugin's message context is
	  executed. Experimental and unsupported. Don't use it.
      */
      bool blocking_run(uint8_t* outputs_written) { return false; }
      
    protected:
      
      /** @internal
	  Static callback wrapper. */
      static bool _blocking_run(LV2_Handle h, uint8_t* outputs_written) {
	return reinterpret_cast<Derived*>(h)->blocking_run(outputs_written);
      }
      
      /** @internal
	  Static callback wrapper. */
      static void _connect_port(LV2_Handle h, uint32_t port, void* buffer) {
	reinterpret_cast<Derived*>(h)->connect_port(port, buffer);
      }
      
    };
  };


}


#endif
