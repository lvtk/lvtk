/****************************************************************************
    
    lv2plugin.hpp - support file for writing LV2 plugins in C++
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
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

#include <lv2.h>
#include <lv2-command.h>


namespace LV2 {
  
  /** Typedef for the LV2_Feature type so we get it into the LV2 namespace. */
  typedef LV2_Feature Feature;
  
  /** Convenient typedef for the feature handler function type. */
  typedef void(*FeatureHandler)(void*, void*);
  
  /** Convenient typedef for the feature handler map type. */
  typedef std::map<std::string, FeatureHandler> FeatureHandlerMap;
  
  

  /** @internal
      This template class is used to terminate the recursive inheritance trees
      created by InheritanceTree. */
  template <class A, bool B>
  struct End {
    
  };
  

  /** @internal
      This template class creates an inheritance tree of extension templates
      from a parameter list. It is inherited by the Plugin class to make
      it possible to add overridable extension functions to the class.
      The first template parameter will be used as the first template
      parameter of @c E1, and also be passed as the first parameter of the
      next level of the inheritance tree. Each @c bool parameter will be used
      as the second parameter to the template directly preceding it. */
  template <class A,
	    template <class, bool> class E1 = End, bool B1 = false,
	    template <class, bool> class E2 = End, bool B2 = false,
	    template <class, bool> class E3 = End, bool B3 = false,
	    template <class, bool> class E4 = End, bool B4 = false,
	    template <class, bool> class E5 = End, bool B5 = false,
	    template <class, bool> class E6 = End, bool B6 = false,
	    template <class, bool> class E7 = End, bool B7 = false,
	    template <class, bool> class E8 = End, bool B8 = false,
	    template <class, bool> class E9 = End, bool B9 = false>
  struct InheritanceTree 
    : E1<A, B1>, InheritanceTree<A, E2, B2, E3, B3, E4, B4, E5, B5, 
				 E6, B6, E7, B7, E8, B8, E9, B9> {
    
    typedef InheritanceTree<A, E2, B2, E3, B3, E4, B4, E5, B5, 
			    E6, B6, E7, B7, E8, B8, E9, B9> Parent;
    
    /** @internal
	Add feature handlers to @c hmap for the feature URIs. */
    static void map_feature_handlers(FeatureHandlerMap& hmap) {
      E1<A, B1>::map_feature_handlers(hmap);
      Parent::map_feature_handlers(hmap);
    }
    
    /** Check if the features are OK with the plugin initialisation. */
    bool check_ok() const { 
      return E1<A, B1>::check_ok() && Parent::check_ok();
    }
    
    /** Return any extension data. */
    static const void* extension_data(const char* uri) {
      const void* result = E1<A, B1>::extension_data(uri);
      if (result)
	return result;
      return Parent::extension_data(uri);
    }
    
  };


  /** @internal
      This is a specialisation of the inheritance tree template that terminates
      the recursion. */
  template <class A>
  struct InheritanceTree<A, 
			 End, false, End, false, End, false, 
			 End, false, End, false, End, false, 
			 End, false, End, false, End, false> {
    static void map_feature_handlers(FeatureHandlerMap& hmap) { }
    bool check_ok() const { return true; }
    static const void* extension_data(const char* uri) { return 0; }
  };

  
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
      otherwise it will not work with the template class LV2::Register. The 
      host will use these parameter to pass the sample rate, the path to the 
      bundle directory and the list of features passed from the host when it 
      creates a new instance of the plugin. 
      
      This is a template so that simulated dynamic binding can be used for
      the callbacks. This is not all that useful for simple plugins but it may
      come in handy for extensions and it doesn't add any additional vtable
      lookup and function call costs, like real dynamic binding would.
      @code
      #include <lv2plugin.hpp>
      
      class TestLV2 : public LV2::Plugin<TestLV2> {
      public:
        TestLV2(double, const char*, const LV2::Feature* const*) : LV2::Plugin<TestLV2>(2) { }
        void run(uint32_t sample_count) {
          memcpy(p(1), p(0), sample_count * sizeof(float));
        }
      };
      
      static unsigned _ = TestLV2::register_class<TestLV2>("http://ll-plugins.sf.net/plugins/TestLV2#0.0.0");
      @endcode
      
      If the above code is compiled and linked with @c -llv2_plugin into a 
      shared module, it could form the shared object part of a fully 
      functional (but not very useful) LV2 plugin with one audio input port
      and one audio output port that just copies the input to the output.
  */
  template <class Derived, 
	    template <class, bool> class Ext1 = End, bool Req1 = false,
	    template <class, bool> class Ext2 = End, bool Req2 = false,
	    template <class, bool> class Ext3 = End, bool Req3 = false,
	    template <class, bool> class Ext4 = End, bool Req4 = false,
	    template <class, bool> class Ext5 = End, bool Req5 = false,
	    template <class, bool> class Ext6 = End, bool Req6 = false,
	    template <class, bool> class Ext7 = End, bool Req7 = false,
	    template <class, bool> class Ext8 = End, bool Req8 = false,
	    template <class, bool> class Ext9 = End, bool Req9 = false>
  class Plugin : public InheritanceTree<Derived, 
				      Ext1, Req1, Ext2, Req2, Ext3, Req3,
				      Ext4, Req4, Ext5, Req5, Ext6, Req6,
				      Ext7, Req7, Ext8, Req8, Ext9, Req9> {
  public:
    
    /** This constructor is needed to initialise the port vector with the
	correct number of ports, and to check if all the required features
	are provided. */
    Plugin(uint32_t ports, const Feature* const* f = 0) 
      : m_ports(ports, 0) {
      if (f) {
	FeatureHandlerMap hmap;
	Derived::map_feature_handlers(hmap);
	for (const Feature* const* iter = f; *iter != 0; ++iter) {
	  FeatureHandlerMap::iterator miter;
	  miter = hmap.find((*iter)->URI);
	  if (miter != hmap.end())
	    miter->second(static_cast<Derived*>(this), (*iter)->data);
	}
      }
    }
    
    /** Connects the ports. You shouldn't have to override this, just use
	p(port) to access the port buffers. 
    
	If you do override this function, remember that if you want your plugin
	to be realtime safe this function may not block, allocate memory or
	otherwise take a long time to return. */
    void connect_port(uint32_t port, void* data_location) {
      m_ports[port] = data_location;
    }
    
    /** Override this function if you need to do anything on activation. 
	This is always called before the host starts using the run() function.
	You should reset your plugin to it's initial state here. */
    void activate() { }
  
    /** This is the process callback which should fill all output port buffers. 
	You most likely want to override it. 
	
	Remember that if you want your plugin to be realtime safe, this function
	may not block, allocate memory or take more than O(sample_count) time
	to execute. */
    void run(uint32_t sample_count) { }
  
    /** Override this function if you need to do anything on deactivation. 
	The host calls this when it does not plan to make any more calls to 
	run() (unless it calls activate() again). */
    void deactivate() { }
    
  //  protected:
  
    /** Use this function to access and cast port buffers, for example
        like this:
    
        @code
LV2_MIDI* midibuffer = p<LV2_MIDI>(midiport_index);
        @endcode
	
	If you want to access a port buffer as a pointer-to-float (i.e. an audio
	or control port) you can use the non-template version instead. */
    template <typename T> T*& p(uint32_t port) {
      return reinterpret_cast<T*&>(m_ports[port]);
    }
  
    /** Use this function to access data buffers for control or audio ports. */
    float*& p(uint32_t port) {
      return reinterpret_cast<float*&>(m_ports[port]);
    }

    /** @internal
	This vector contains pointers to all port buffers. You don't need to
	access it directly, use the p() function instead. */
    std::vector<void*> m_ports;


  public:

    static void _connect_port(LV2_Handle instance, uint32_t port, 
			     void* data_location) {
      reinterpret_cast<Derived*>(instance)->connect_port(port, data_location);
    }
  
    static void _activate(LV2_Handle instance) {
      reinterpret_cast<Derived*>(instance)->activate();
    }
  
    static void _run(LV2_Handle instance, uint32_t sample_count) {
      reinterpret_cast<Derived*>(instance)->run(sample_count);
    }
  
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
					      features) {
      Derived* t = new Derived(sample_rate, bundle_path, features);
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


    /** Use this function to register your plugin class so that the host
	can find it. You need to do this when the shared library is loaded 
	by the host. One way of doing that is to put the function call in 
	the initialiser for a global variable, like this:
	
	@code
unsigned _ =  MyPluginClass::register_class("http://my.plugin.class");
        @endcode
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


  };
  
  
  /** Base class for extensions. Extension classes don't have to inherit from
      this class, but it's convenient. */
  template <bool Required>
  struct Extension {
    
    Extension() : m_ok(!Required) { }
    
    /** Default implementation does nothing - no handlers added. */
    static void map_feature_handlers(FeatureHandlerMap& hmap) { }
    
    /** Return @c true if the plugin instance is OK, @c false if it isn't. */
    bool check_ok() const { return m_ok; }
  
    /** Return a data pointer corresponding to the URI if this extension 
	has one. */
    static const void* extension_data(const char* uri) { return 0; }
  
  protected:
  
    bool m_ok;
  
  };

  
  /** @internal
      The URI used for the Command extension. */
  static const char* command_uri = 
		 "http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension";
  
  
  /** The Command extension. Deprecated, but still used. */
  template <class Derived, bool Required>
  struct CommandExt : Extension<Required> {
    
    CommandExt() : m_chd(0) { }
    
    static void map_feature_handlers(FeatureHandlerMap& hmap) {
      hmap[command_uri] = &CommandExt::handle_feature;
    }
    
    static void handle_feature(void* instance, void* data) { 
      Derived* d = reinterpret_cast<Derived*>(instance);
      CommandExt* ce = static_cast<CommandExt*>(d);
      ce->m_chd = reinterpret_cast<LV2_CommandHostDescriptor*>(data);
      ce->m_ok = (data != 0);
    }
  
    static const void* extension_data(const char* uri) { 
      if (!std::strcmp(uri, command_uri)) {
	static LV2_CommandDescriptor cdesc = { &CommandExt::_command };
	return &cdesc;
      }
      return 0;
    }
    
    /** This function is called by the host when the GUI sends a command
	to the plugin. You should override it in your plugin if you want
	to do something when you get a command. */
    char* command(uint32_t argc, const char* const* argv) { }
    
    /** @internal
	Static callback wrapper. */
    static char* _command(LV2_Handle h, 
			  uint32_t argc, const char* const* argv) {
      reinterpret_cast<Derived*>(h)->command(argc, argv);
    }

  protected:
    
    /** This function sends a message to the host, which sends it to the
	GUI. Use it to send feedback to the GUI when reacting to commands. */
    void feedback(uint32_t argc, const char* const* argv) {
      (*m_chd->feedback)(m_chd->host_data, argc, argv);
    }
    
    /** Convenient wrapper for the other feedback() function. The first 
	parameter is a OSC-like type signature, where "s" means @c char*,
	"i" means @c long and "f" means @c double. */
    void feedback(const std::string& types, ...) {
      va_list ap;
      va_start(ap, types);
      uint32_t argc = types.size();
      char** argv = static_cast<char**>(malloc(sizeof(char*) * argc));
      for (uint32_t i = 0; i < argc; ++i) {
	if (types[i] == 's')
	  argv[i] = strdup(va_arg(ap, const char*));
	else if (types[i] == 'i') {
	  std::ostringstream oss;
	  oss<<va_arg(ap, long);
	  argv[i] = strdup(oss.str().c_str());
	}
	else if (types[i] == 'f') {
	  std::ostringstream oss;
	  oss<<va_arg(ap, double);
	  argv[i] = strdup(oss.str().c_str());
	}
      }
      va_end(ap);
      feedback(argc, argv);
    }
    
    /** @internal
	The host descriptor as defined by the Command extension
	specification. */
    LV2_CommandHostDescriptor* m_chd;
    
  };
  
  
  static const char* fixed_uri = 
    "http://tapas.affenbande.org/lv2/ext/fixed-buffersize";
  
  /** The fixed buffer size extension. A host that supports this will always
      call the plugin's run() function with the same @c sample_count parameter,
      which will be equal to the uint32_t variable pointed to by the data
      pointer for this feature. */
  template <class Derived, bool Required>
  struct FixedExt : Extension<Required> {
    
    FixedExt() : m_buffer_size(0) { }
    
    static void map_feature_handlers(FeatureHandlerMap& hmap) {
      hmap[fixed_uri] = &FixedExt::handle_feature;
    }
    
    static void handle_feature(void* instance, void* data) { 
      Derived* d = reinterpret_cast<Derived*>(instance);
      FixedExt* fe = static_cast<FixedExt*>(d);
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


  static const char* fixedp2_uri = 
    "http://tapas.affenbande.org/lv2/ext/power-of-two-buffersize";
  
  /** The fixed power-of-2 buffer size extension. This works just like 
      FixedExt with the additional requirement that the buffer size must
      be a power of 2. */
  template <class Derived, bool Required>
  struct FixedP2Ext : Extension<Required> {
    
    FixedP2Ext() : m_buffer_size(0) { }
    
    static void map_feature_handlers(FeatureHandlerMap& hmap) {
      hmap[fixedp2_uri] = &FixedP2Ext::handle_feature;
    }
    
    static void handle_feature(void* instance, void* data) { 
      Derived* d = reinterpret_cast<Derived*>(instance);
      FixedP2Ext* fe = static_cast<FixedP2Ext*>(d);
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

}


#endif
