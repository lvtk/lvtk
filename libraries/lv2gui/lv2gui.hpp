/****************************************************************************
    
    lv2gui.hpp - Wrapper library to make it easier to write LV2 GUIs in C++
    
    Copyright (C) 2006-2008 Lars Luthman <lars.luthman@gmail.com>
    Modified by Dave Robillard, 2008 (URI map mixin)
    
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef LV2GUI_HPP
#define LV2GUI_HPP

#include <map>

#include <gtkmm/box.h>
#include <gtkmm/main.h>
#include <gtkmm/widget.h>

#include <lv2-ui.h>
#include <lv2-gui-programs.h>
#include <lv2-ui-command.h>
#include <lv2_uri_map.h>
#include <lv2_event_helpers.h>
#include <lv2types.hpp>


namespace LV2 {

  
  /** @internal
      A convenient typedef that is only used internally. */
  typedef std::vector<LV2UI_Descriptor*> GUIDescList;
    
  
  /** @internal
      This function returns the list of LV2 descriptors. It should only be 
      used internally. */
  GUIDescList& get_lv2g2g_descriptors();


  /** This extension provides no extra functions or data, it just makes sure
      that the GUI will not instantiate unless the host passes a LV2_Feature
      for the noUserResize GUI feature defined in the GUI extension (if
      @c Required is @c true).
  */
  template <bool Required>
  struct NoUserResize {
    
    template <class Derived> struct I : public Extension<Required> {
      
      /** @internal */
      static void map_feature_handlers(FeatureHandlerMap& hmap) {
	hmap["http://ll-plugins.nongnu.org/lv2/dev/ui#noUserResize"] = 
	  &I<Derived>::handle_feature;
      }
      
      /** @internal */
      static void handle_feature(void* instance, void* data) { 
	Derived* d = reinterpret_cast<Derived*>(instance);
	I<Derived>* e = static_cast<I<Derived>*>(d);
	e->m_ok = true;
      }
      
    };
    
  };


  /** This extension provides no extra functions or data, it just makes sure
      that the GUI will not instantiate unless the host passes a LV2_Feature
      for the fixedSize GUI feature defined in the GUI extension (if
      @c Required is @c true).
  */
  template <bool Required>
  struct FixedSize {
    
    template <class Derived> struct I : public Extension<Required> {
      
      /** @internal */
      static void map_feature_handlers(FeatureHandlerMap& hmap) {
	hmap["http://ll-plugins.nongnu.org/lv2/dev/ui#fixedSize"] = 
	  &I<Derived>::handle_feature;
      }
      
      /** @internal */
      static void handle_feature(void* instance, void* data) {
	Derived* d = reinterpret_cast<Derived*>(instance);
	I<Derived>* e = static_cast<I<Derived>*>(d);
	e->m_ok = true;
      }
    
    };
    
  };


  /** Program GUI extension - the host will tell the GUI what presets are
    available and which is currently active, the GUI can request saving
    and using programs.
  */
  template <bool Required>
  struct Programs {
    
    template <class Derived> struct I : public Extension<Required> {
      
      /** @internal */
      I() : m_hdesc(0), m_host_support(false) { }
	
	/** @internal */
	static void map_feature_handlers(FeatureHandlerMap& hmap) {
	  hmap[LV2_UI_URI "#ext_programs"] = 
	    &I<Derived>::handle_feature;
	}
	
	/** @internal */
	static void handle_feature(void* instance, void* data) {
	  Derived* d = reinterpret_cast<Derived*>(instance);
	  I<Derived>* e = static_cast<I<Derived>*>(d);
	  e->m_hdesc = static_cast<LV2UI_Programs_HDesc*>(data);
	  e->m_ok = (e->m_hdesc != 0);
	  e->m_host_support = (e->m_hdesc != 0);
	}
	
	
	/** This is called by the host to let the GUI know that a new 
	    program has been added or renamed.
	*/
	void program_added(uint32_t    number, 
			   char const* name) {
	  
	}
	
	/** This is called by the host to let the GUI know that a previously 
	    existing program has been removed.
	*/
	void program_removed(uint32_t number) {
	  
	}
	
	/** This is called by the host to let the GUI know that all previously
	    existing programs have been removed. 
	*/
	void programs_cleared() {
	  
	}
	
	/** This is called by the host to let the GUI know that the current 
	    program has changed. The number will always be in the range [0,127]
	    or equal to 255, in which case there is no current program.
	*/
	void current_program_changed(uint32_t number) {
	  
	}
	
	/** @internal
	    Returns the plugin descriptor for this extension.
	*/
	static void const* extension_data(char const* uri) {
	  static LV2UI_Programs_GDesc desc = { &_program_added,
					       &_program_removed,
					       &_programs_cleared,
					       &_current_program_changed };
	  if (!std::strcmp(uri, LV2_UI_URI "#ext_programs"))
	    return &desc;
	  return 0;
	}
	
    protected:
	
	/** You can call this to request that the host changes the current 
	    program to @c program. */
	void change_program(uint32_t program) {
	  if (m_hdesc)
	    m_hdesc->change_program(static_cast<Derived*>(this)->controller(), 
				    program);
	}
	
	/** You can call this to request that the host saves the current state
	    of the plugin instance to a program. */
	void save_program(uint32_t program, char const* name) {
	  if (m_hdesc)
	    m_hdesc->save_program(static_cast<Derived*>(this)->controller(), 
				  program, name);
	}
      
        /** Returns @c true if the host supports the Program feature, @c false
	    if it does not. */
        bool host_supports_programs() const {
	  return m_host_support;
	}
	
    private:
	
	static void _program_added(LV2UI_Handle gui, 
				   uint32_t     number, 
				   char const*  name) {
	  static_cast<Derived*>(gui)->program_added(number, name);
	}
	
	static void _program_removed(LV2UI_Handle gui, 
				     uint32_t     number) {
	  static_cast<Derived*>(gui)->program_removed(number);
	}
	
	static void _programs_cleared(LV2UI_Handle gui) {
	  static_cast<Derived*>(gui)->programs_cleared();
	}
	
	static void _current_program_changed(LV2UI_Handle gui, 
					     uint32_t     number) {
	  static_cast<Derived*>(gui)->current_program_changed(number);
	}
	
	
	LV2UI_Programs_HDesc* m_hdesc;
        bool m_host_support;
	
    };

  };

  
  /** Command GUI extension - the GUI can send commands to the plugin,
      and the plugin can send feedback back to the GUI.
  */
  template <bool Required>
  struct CommandGUI {
    
    template <class Derived> struct I : public Extension<Required> {
      
      /** @internal */
      I() : m_hdesc(0), m_host_support(false) { }
      
      /** @internal */
      static void map_feature_handlers(FeatureHandlerMap& hmap) {
	hmap[LV2_UI_URI "#ext_command"] = 
	  &I<Derived>::handle_feature;
      }
      
      /** @internal */
      static void handle_feature(void* instance, void* data) {
	Derived* d = reinterpret_cast<Derived*>(instance);
	I<Derived>* e = static_cast<I<Derived>*>(d);
	e->m_hdesc = static_cast<LV2UI_Command_HDesc*>(data);
	e->m_ok = (e->m_hdesc != 0);
	e->m_host_support = (e->m_hdesc != 0);
      }
      
      
      /** This is called by the host to send feedback to the GUI.
       */
      void feedback(uint32_t argc, char const* const* argv) {
	
      }
      
      
      /** @internal
	  Returns the plugin descriptor for this extension.
      */
      static void const* extension_data(char const* uri) {
	static LV2UI_Command_GDesc desc = { &_feedback };
	if (!std::strcmp(uri, LV2_UI_URI "#ext_command"))
	  return &desc;
	return 0;
      }
      
    protected:
	
      /** You can call this to send a command to the plugin. */
      void command(uint32_t argc, char const* const* argv) {
	if (m_hdesc)
	  m_hdesc->command(static_cast<Derived*>(this)->controller(), 
			   argc, argv);
      }
      
      
      /** Returns @c true if the host supports the Command feature, @c false
	  if it does not. */
      bool host_supports_commands() const {
	return m_host_support;
      }
      
    private:
      
      static void _feedback(LV2UI_Handle gui, 
			    uint32_t argc, char const* const* argv) {
	static_cast<Derived*>(gui)->feedback(argc, argv);
      }

      LV2UI_Command_HDesc* m_hdesc;
      bool m_host_support;
      
    };
    
  };


  /** The URI map extension. */
  template <bool Required>
  struct UriMapExt {
    
    template <class Derived> struct I : Extension<Required> {
      
      I() : m_uri_map_feature(0) { }
      
      static void map_feature_handlers(FeatureHandlerMap& hmap) {
	hmap[LV2_URI_MAP_URI] = &I<Derived>::handle_feature;
      }
      
      static void handle_feature(void* instance, void* data) { 
	Derived* d = reinterpret_cast<Derived*>(instance);
	I<Derived>* fe = static_cast<I<Derived>*>(d);
	fe->m_uri_map_feature = reinterpret_cast<LV2_URI_Map_Feature*>(data);
	fe->m_ok = true;
      }
      
    protected:
      
      LV2_URI_Map_Feature* m_uri_map_feature;

      uint32_t uri_to_id(const char* map, const char* uri) const {
	return m_uri_map_feature->uri_to_id(m_uri_map_feature->callback_data, 
					    map, uri);
      }
      
    };
    
  };  
  
  
  /** A mixin that allows easy sending of MIDI from GUI to plugin. */
  struct WriteMidi {
    
    template <class Derived> struct I : Extension<false> {
      
      I() : m_midi_type(0) {
	m_buffer = lv2_event_buffer_new(sizeof(LV2_Event) + 4, 0);
      }
      
      bool check_ok() {
	Derived* d = static_cast<Derived*>(this);
	m_midi_type = d->
	  uri_to_id(LV2_EVENT_URI, "http://lv2plug.in/ns/ext/midi#MidiEvent");
	m_event_buffer_format = 1; // XXX Fix this
	return m_midi_type && m_event_buffer_format;
      }
      
    protected:
      
      void write_midi(uint32_t port, uint32_t size, const uint8_t* data) {
	if (size > 4)
	  return;
	lv2_event_buffer_reset(m_buffer, 0, m_buffer->data);
	LV2_Event_Iterator iter;
	lv2_event_begin(&iter, m_buffer);
	lv2_event_write(&iter, 0, 0, m_midi_type, size, data);
	static_cast<Derived*>(this)->
	  write(port, m_buffer->header_size + m_buffer->capacity, 
		m_event_buffer_format, m_buffer);
      }
      
      uint32_t m_midi_type;
      uint32_t m_event_buffer_format;
      LV2_Event_Buffer* m_buffer;
      
    };
    
  };  
  
  
  /** This is the base class for a plugin GUI. You should inherit it and 
      override any public member functions you want to provide 
      implementations for. All subclasses must have a constructor with 
      the signature
      
      MyGUI(char const* plugin_uri, char const* bundle_path);
	    
      where @c plugin_uri is the URI of the plugin that this GUI will
      control (not the URI for the GUI itself) and @c bundle_path is
      the filesystem path to the LV2 bundle that contains this GUI.
  */
  template<class Derived, class Ext1 = End, class Ext2 = End, class Ext3 = End,
           class Ext4 = End, class Ext5 = End, class Ext6 = End,
	   class Ext7 = End, class Ext8 = End, class Ext9 = End>
  class GUI : public Gtk::HBox, public MixinTree<Derived, 
						 Ext1, Ext2, Ext3, Ext4, 
						 Ext5, Ext6, Ext7, Ext8, Ext9> {
  public:
    
    /** @internal
	The constructor for the GUI class. You should never use it directly.
    */
    inline GUI() {
      m_ctrl = s_ctrl;
      m_wfunc = s_wfunc;
      m_features = s_features;
      m_bundle_path = s_bundle_path;
      s_ctrl = 0;
      s_wfunc = 0;
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
    
    /** Override this if you want your GUI to do something when a control port
	value changes in the plugin instance. */
    inline void port_event(uint32_t port, uint32_t buffer_size, 
			   uint32_t format, void const* buffer) { }
    
    /** Use this template function to register a class as a LV2 GUI. */
    static int register_class(char const* uri) {
      LV2UI_Descriptor* desc = new LV2UI_Descriptor;
      std::memset(desc, 0, sizeof(LV2UI_Descriptor));
      desc->URI = strdup(uri);
      desc->instantiate = &Derived::create_ui_instance;
      desc->cleanup = &Derived::delete_ui_instance;
      desc->port_event = &Derived::_port_event;
      desc->extension_data = &Derived::extension_data;
      get_lv2g2g_descriptors().push_back(desc);
      return get_lv2g2g_descriptors().size() - 1;
    }
    
  protected:
    
    /** Send a chunk of data to a plugin port. The format of the chunk is
	determined by the port type and the transfer mechanisms used, you
	should probably use a wrapper function instead such as write_control().
    */
    inline void write(uint32_t port, uint32_t buffer_size, 
		      uint32_t format, void const* buffer) {
      (*m_wfunc)(m_ctrl, port, buffer_size, format, buffer);
    }
    
    /** Set the value of a control input port in the plugin instance. */
    inline void write_control(uint32_t port, float value) {
      write(port, sizeof(float), 0, &value);
    }
    
    /** Get the feature array that was passed by the host. This may only
	be valid while the constructor is running. */
    inline LV2::Feature const* const* features() {
      return m_features;
    }
    
    /** Get the filesystem path to the bundle that contains this GUI. */
    inline char const* bundle_path() const {
      return m_bundle_path;
    }
    
  public:
    /** Get the controller - a handle on the plugin instance this GUI
	is controlling. You only need it if you want to handle extensions
	yourself. */
    inline void* controller() {
      return m_ctrl;
    }
    
    
  private:
    
    // XXX This is quite ugly but needed to allow these mixins to call 
    // protected functions in the GUI class, which we want.
    friend class WriteMidi::I<Derived>;
    //friend class Ext1::template I<Derived>;
    //friend class Ext2::template I<Derived>;
    //friend class Ext3::template I<Derived>;
    //friend class Ext4::template I<Derived>;
    //friend class Ext5::template I<Derived>;
    //friend class Ext6::template I<Derived>;
    //friend class Ext7::template I<Derived>;
    //friend class Ext8::template I<Derived>;
    //friend class Ext9::template I<Derived>;
    
    /** @internal
	This function creates an instance of a plugin GUI. It is used 
	as the instantiate() callback in the LV2 descriptor. You should not use
	it directly. */
    static LV2UI_Handle create_ui_instance(struct _LV2UI_Descriptor const* 
					   descriptor,
					   char const* plugin_uri,
					   char const* bundle_path,
					   LV2UI_Write_Function write_func,
					   LV2UI_Controller ctrl,
					   LV2UI_Widget* widget,
					   LV2_Feature const* const* features) {
      
      // copy some data to static variables so the subclasses don't have to
      // bother with it
      s_ctrl = ctrl;
      s_wfunc = write_func;
      s_features = features;
      s_bundle_path = bundle_path;
      
      // this is needed to initialise gtkmm stuff in case we're running in
      // a Gtk+ or PyGtk host or some other language
      Gtk::Main::init_gtkmm_internals();
      
      // create the GUI object
      Derived* t = new Derived(plugin_uri);
      *widget = static_cast<Gtk::Widget*>(t)->gobj();
      
      // check that everything is OK
      if (t->check_ok())
	return reinterpret_cast<LV2UI_Handle>(t);
      delete t;
      return 0;
    }
    

    /** @internal
	This function destroys an instance of a GUI. It is used as the
	cleanup() callback in the LV2UI descriptor. You should not use it 
	directly. */
    static void delete_ui_instance(LV2UI_Handle instance) {
      delete static_cast<Derived*>(instance);
    }
    
    
    /** @internal
	This is the main port_event() callback. You should not use it directly.
    */
    static void _port_event(LV2UI_Handle instance, uint32_t port, 
			    uint32_t buffer_size, uint32_t format, 
			    void const* buffer) {
      static_cast<Derived*>(instance)->port_event(port, buffer_size, 
						  format, buffer);
    }
    

    void* m_ctrl;
    LV2UI_Write_Function m_wfunc;
    LV2::Feature const* const* m_features;
    char const* m_bundle_path;
    
    static void* s_ctrl;
    static LV2UI_Write_Function s_wfunc;
    static LV2::Feature const* const* s_features;
    static char const* s_bundle_path;
    
  };

  
  /* Yes, static variables are messy. */
  template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
           class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
  void* GUI<Derived, Ext1, Ext2, Ext3, Ext4, 
	    Ext5, Ext6, Ext7, Ext8, Ext9>::s_ctrl = 0; 
  
  template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
           class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
  LV2UI_Write_Function GUI<Derived, Ext1, Ext2, Ext3, Ext4, 
			   Ext5, Ext6, Ext7, Ext8, Ext9>::s_wfunc = 0;
  
  template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
           class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
  LV2::Feature const* const* GUI<Derived, Ext1, Ext2, Ext3, Ext4, 
				 Ext5, Ext6, Ext7, Ext8, Ext9>::s_features = 0;
  
  template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
           class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
  char const* GUI<Derived, Ext1, Ext2, Ext3, Ext4, 
		  Ext5, Ext6, Ext7, Ext8, Ext9>::s_bundle_path = 0;
  

}


#endif
