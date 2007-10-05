/****************************************************************************
    
    lv2advanced.hpp - support file for writing LV2 plugins in C++
                        using the Advanced extension
    
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

#ifndef LV2ADVANCED_HPP
#define LV2ADVANCED_HPP

#include <unistd.h>


#include <iostream>
#include <cstring>
#include <sstream>
#include <stdarg.h>
#include <string>
#include <utility>
#include <vector>

#include "lv2plugin.hpp"
#include "lv2-command.h"


namespace LV2 {


  class Advanced : public Plugin {
  public:
  
    /** This constructor is needed to initialise the port vector with the
	correct number of ports. */
    Advanced(uint32_t ports) 
      : Plugin(ports),
	m_feedback_real(0),
	m_host_data(0) { }
  
    /** Function for sending commands to the plugin. */
    char* command(uint32_t argc, const char*const* argv) { return 0; }
  
  protected:
  
    void feedback(uint32_t argc, const char* const* argv) {
    
      std::cerr<<__PRETTY_FUNCTION__<<std::endl;
    
      if (m_feedback_real)
	(*m_feedback_real)(m_host_data, argc, argv);
    }
  
    void feedback() {
      feedback(0, 0);
    }
  
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
  
    void (*m_feedback_real)(void*, uint32_t, const char* const*);
    void* m_host_data;

  public:
  
    template <class T>
    static char* command(LV2_Handle instance, 
			 uint32_t argc, const char* const* argv) {
      return reinterpret_cast<T*>(instance)->command(argc, argv);
    }
    
    template <class T>
    static void* lv2_advanced_descriptor(const char* URI) {
      if (!std::strcmp(URI, "<http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension>")) {
	static LV2_CommandDescriptor cdesc = {
	  &Advanced::command<T>
	};
	return &cdesc;
      }
      return 0;
    }

  
  
    /* This template function creates an instance of a plugin. It is used as
       the instantiate() callback in the LV2 descriptor. You should not use
       it directly. It will return 0 if the host does not provide the 
       Command extension. */
    template <class T>
    static LV2_Handle create_advanced_instance(const LV2_Descriptor* descriptor,
					       double sample_rate,
					       const char* bundle_path,
					       const LV2_Host_Feature* const* host_features) {
      // check that the host supports the Advanced extension
      size_t i;
      LV2_CommandHostDescriptor* chd = 0; 
      for (i = 0; host_features[i]; ++i) {
	if (!std::strcmp(host_features[i]->URI, 
			 "<http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension>")) {
	  chd = static_cast<LV2_CommandHostDescriptor*>(host_features[i]->data);
	  break;
	}
      }
      if (!chd)
	return 0;
    
      // create and return an instance of the plugin
      T* t = new T(sample_rate, bundle_path, host_features);
      static_cast<Advanced*>(t)->m_feedback_real = chd->feedback;
      static_cast<Advanced*>(t)->m_host_data = chd->host_data;
    
      return reinterpret_cast<LV2_Handle>(t);
    }

  };


  template <class T>
  class RegisterAdvanced {
  public:
    RegisterAdvanced(const std::string& uri) {
      LV2_Descriptor desc;
      std::memset(&desc, 0, sizeof(LV2_Descriptor));
      char* c_uri = new char[uri.size() + 1];
      std::memcpy(c_uri, uri.c_str(), uri.size() + 1);
      desc.URI = c_uri;
      desc.instantiate = &Advanced::create_advanced_instance<T>;
      desc.connect_port = &Plugin::connect_port<T>;
      desc.activate = &Plugin::activate<T>;
      desc.run = &Plugin::run<T>;
      desc.deactivate = &Plugin::deactivate<T>;
      desc.cleanup = &Plugin::delete_plugin_instance<T>;
      desc.extension_data = &Advanced::lv2_advanced_descriptor<T>;
      Plugin::get_lv2_descriptors().push_back(desc);
    }
  };


}

#endif
