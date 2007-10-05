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

#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include <lv2.h>


namespace LV2 {

  /** This is a base class for LV2 plugins. It has default implementations for
      all functions, so you only have to implement the functions that you need
      (for example run()). All subclasses must have a constructor that takes
      a single <code>double</code> as parameter, otherwise it will 
      not work with the template class LV2::Register. The host will use 
      this parameter to pass the sample rate when it creates a new instance of
      the plugin. 
      
      Here's an example of a plugin that simply copies the input to the output:
      
      @code
      #include "lv2plugin.hpp"
      
      
      class TestLV2 : public LV2::Plugin {
      public:
      TestLV2(uint32_t, const char*, const LV2_Host_Feature**) : LV2::Plugin(2) { }
      void run(uint32_t sample_count) {
      memcpy(m_ports[1], m_ports[0], sample_count * sizeof(float));
      }
      };
      
      static LV2::Register register("http://ll-plugins.sf.net/plugins/TestLV2#0.0.0");
      @endcode
      
      If the above code is compiled and linked with @c -llv2_plugin into a 
      shared module, it should be a fully functional (but not very useful) LV2 
      plugin.
  */
  class Plugin {
  public:
    
    /** This constructor is needed to initialise the port vector with the
	correct number of ports. */
    Plugin(uint32_t ports) : m_ports(ports, 0) { }
    
    /** Connects the ports. You shouldn't have to override this, just use
	p(port) to access the port buffers. */
    void connect_port(uint32_t port, void* data_location) {
      m_ports[port] = data_location;
    }
    
    /** Override this function if you need to do anything on activation. 
	This is always called before the host starts using the run() function.
	You should reset your plugin to it's initial state here. */
    void activate() { }
  
    /** This is the process callback which should fill all output port buffers. 
	You most likely want to override it. */
    void run(uint32_t sample_count) { }
  
    /** Override this function if you need to do anything on deactivation. 
	The host calls this when it does not plan to make any more calls to 
	run() (unless it calls activate() again). */
    void deactivate() { }

  protected:
  
    /** Use this as a shorthand to access and cast port buffers. */
    template <typename T> inline T*& p(uint32_t port) {
      return reinterpret_cast<T*&>(m_ports[port]);
    }
  
    /** This is needed because default template parameters aren't allowed for
	template functions. */
    float*& p(uint32_t port) {
      return reinterpret_cast<float*&>(m_ports[port]);
    }
  
    /** This vector contains pointers to all port buffers. You don't need to
	access it directly, use the p() function instead. */
    std::vector<void*> m_ports;


  public:

    template <class T>
    static void connect_port(LV2_Handle instance, uint32_t port, 
			     void* data_location) {
      reinterpret_cast<T*>(instance)->connect_port(port, data_location);
    }
  
    template <class T>
    static void activate(LV2_Handle instance) {
      reinterpret_cast<T*>(instance)->activate();
    }
  
    template <class T>
    static void run(LV2_Handle instance, uint32_t sample_count) {
      reinterpret_cast<T*>(instance)->run(sample_count);
    }
  
    template <class T>
    static void deactivate(LV2_Handle instance) {
      reinterpret_cast<T*>(instance)->deactivate();
    }
  
    /* This template function creates an instance of a plugin. It is used as
       the instantiate() callback in the LV2 descriptor. You should not use
       it directly. */
    template <class T>
    static LV2_Handle create_plugin_instance(const LV2_Descriptor* descriptor,
					     double sample_rate,
					     const char* bundle_path,
					     const LV2_Host_Feature* const* 
					     host_features) {
      T* t = new T(sample_rate, bundle_path, host_features);
      return reinterpret_cast<LV2_Handle>(t);
    }
  
    /* This function destroys an instance of a plugin. It is used as the
       cleanup() callback in the LV2 descriptor. You should not use it
       directly. */
    template <class T>
    static void delete_plugin_instance(LV2_Handle instance) {
      delete reinterpret_cast<T*>(instance);
    }
  

    typedef std::vector<LV2_Descriptor> DescList;
  
    /** This returns a list of all registered plugins. It is only used 
	internally. */
    static DescList& get_lv2_descriptors();

  };


  template <class T>
  class Register {
  public:
    Register(const std::string& uri) {
      LV2_Descriptor desc;
      std::memset(&desc, 0, sizeof(LV2_Descriptor));
      char* c_uri = new char[uri.size() + 1];
      std::memcpy(c_uri, uri.c_str(), uri.size() + 1);
      desc.URI = c_uri;
      desc.instantiate = &Plugin::create_plugin_instance<T>;
      desc.connect_port = &Plugin::connect_port<T>;
      desc.activate = &Plugin::activate<T>;
      desc.run = &Plugin::run<T>;
      desc.deactivate = &Plugin::deactivate<T>;
      desc.cleanup = &Plugin::delete_plugin_instance<T>;
      Plugin::get_lv2_descriptors().push_back(desc);
    }
  };


}


#endif
