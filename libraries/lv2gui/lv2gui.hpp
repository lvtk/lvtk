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
#include <lv2-ui-presets.h>
#include <lv2-ui-command.h>
#include <lv2_uri_map.h>
#include <lv2_event_helpers.h>
#include <lv2_osc.h>
#include <lv2types.hpp>


namespace LV2 {

  
  /** @internal
      A convenient typedef that is only used internally. */
  typedef std::vector<LV2UI_Descriptor*> GUIDescList;
    
  
  /** @internal
      This function returns the list of LV2 descriptors. It should only be 
      used internally. */
  GUIDescList& get_lv2g2g_descriptors();
  
  
  /** @defgroup guimixins GUI mixins
      These classes implement extra functionality that you may want to have
      in your GUI class, just like the @ref pluginmixins "plugin mixins" do
      for plugin classes. Some of them are template classes with a boolean
      @c Required parameter - if this is true the GUI will fail to instantiate
      unless the host supports the extension implemented by that mixin.
      For example, if you wanted a GUI that wrote a MIDI Note On event to port
      3 in the plugin whenever the user clicked a button, you could do it like
      this:
      @code
#include <lv2gui.hpp>
#include <gtkmm.h>

class MyGUI : public LV2::GUI<MyGUI, LV2::URIMap<true>, LV2::WriteMIDI> {
public:
  MyGUI(const char* plugin_uri)
    : m_button("Click me!") {
    pack_start(m_button);
    m_button.signal_clicked().connect(sigc::mem_fun(*this, &MyGUI::send_event));
  }
protected:
  void send_event() {
    uint8_t noteon[] = { 0x90, 0x50, 0x40 };
    write_midi(3, 3, noteon);
  }
  Gtk::Button m_button;
};
      @endcode
      The function @c write_midi() is implemented in LV2::WriteMIDI and thus
      available in @c MyGUI. LV2::WriteMIDI requires that LV2::URIMap is also
      used (because of the way event types work in LV2) - if you don't add
      LV2::URIMap as well you will get a compilation error.
  */


  /** This extension provides no extra functions or data, it just makes sure
      that the GUI will not instantiate unless the host passes a LV2_Feature
      for the noUserResize GUI feature defined in the GUI extension (if
      @c Required is @c true).
      @ingroup guimixins
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
      @ingroup guimixins
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


  /** Preset GUI extension - the host will tell the GUI what presets are
      available and which is currently active, the GUI can request saving
      and using presets.
      @ingroup guimixins
  */
  template <bool Required>
  struct Presets {
    
    template <class Derived> struct I : public Extension<Required> {
      
      /** @internal */
      I() : m_hdesc(0), m_host_support(false) { }
	
	/** @internal */
	static void map_feature_handlers(FeatureHandlerMap& hmap) {
	  hmap[LV2_UI_PRESETS_URI] = &I<Derived>::handle_feature;
	}
	
	/** @internal */
	static void handle_feature(void* instance, void* data) {
	  Derived* d = reinterpret_cast<Derived*>(instance);
	  I<Derived>* e = static_cast<I<Derived>*>(d);
	  e->m_hdesc = static_cast<LV2UI_Presets_Feature*>(data);
	  e->m_ok = (e->m_hdesc != 0);
	  e->m_host_support = (e->m_hdesc != 0);
	}
	
	
	/** This is called by the host to let the GUI know that a new 
	    preset has been added or renamed.
	*/
	void preset_added(uint32_t    number, 
			   char const* name) {
	  
	}
	
	/** This is called by the host to let the GUI know that a previously 
	    existing preset has been removed.
	*/
	void preset_removed(uint32_t number) {
	  
	}
	
	/** This is called by the host to let the GUI know that all previously
	    existing presets have been removed. 
	*/
	void presets_cleared() {
	  
	}
	
	/** This is called by the host to let the GUI know that the current 
	    preset has changed. The number will always be in the range [0,127]
	    or equal to 255, in which case there is no current preset.
	*/
	void current_preset_changed(uint32_t number) {
	  
	}
	
	/** @internal
	    Returns the plugin descriptor for this extension.
	*/
	static void const* extension_data(char const* uri) {
	  static LV2UI_Presets_GDesc desc = { &_preset_added,
					      &_preset_removed,
					      &_presets_cleared,
					      &_current_preset_changed };
	  if (!std::strcmp(uri, LV2_UI_URI "#ext_presetss"))
	    return &desc;
	  return 0;
	}
	
    protected:
	
	/** You can call this to request that the host changes the current 
	    preset to @c preset. */
	void change_preset(uint32_t preset) {
	  if (m_hdesc)
	    m_hdesc->change_preset(static_cast<Derived*>(this)->controller(), 
				   preset);
	}
	
	/** You can call this to request that the host saves the current state
	    of the plugin instance to a preset. */
	void save_preset(uint32_t preset, char const* name) {
	  if (m_hdesc)
	    m_hdesc->save_preset(static_cast<Derived*>(this)->controller(), 
				 preset, name);
	}
      
        /** Returns @c true if the host supports the Preset feature, @c false
	    if it does not. */
        bool host_supports_presets() const {
	  return m_host_support;
	}
	
    private:
	
	static void _preset_added(LV2UI_Handle gui, 
				   uint32_t     number, 
				   char const*  name) {
	  static_cast<Derived*>(gui)->preset_added(number, name);
	}
	
	static void _preset_removed(LV2UI_Handle gui, 
				     uint32_t     number) {
	  static_cast<Derived*>(gui)->preset_removed(number);
	}
	
	static void _presets_cleared(LV2UI_Handle gui) {
	  static_cast<Derived*>(gui)->presets_cleared();
	}
	
	static void _current_preset_changed(LV2UI_Handle gui, 
					     uint32_t     number) {
	  static_cast<Derived*>(gui)->current_preset_changed(number);
	}
	
	
	LV2UI_Presets_Feature* m_hdesc;
        bool m_host_support;
	
    };

  };

  
  /** Command GUI extension - the GUI can send commands to the plugin,
      and the plugin can send feedback back to the GUI.
      @ingroup guimixins
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


  /** The URI map extension. 
      @ingroup guimixins
  */
  template <bool Required>
  struct URIMap {
    
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
  
  
  /** A mixin that allows easy sending of MIDI from GUI to plugin. 
      @ingroup guimixins
  */
  struct WriteMIDI {
    
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
  
  
  /** A mixin that allows easy sending of OSC from GUI to plugin. 
      @ingroup guimixins
  */
  struct WriteOSC {
    
    template <class Derived> struct I : Extension<false> {
      
      I() : m_osc_type(0) {
	m_buffer = lv2_event_buffer_new(sizeof(LV2_Event) + 256, 0);
      }
      
      bool check_ok() {
	Derived* d = static_cast<Derived*>(this);
	m_osc_type = d->
	  uri_to_id(LV2_EVENT_URI, "http://lv2plug.in/ns/ext/osc#OscEvent");
	m_event_buffer_format = 1; // XXX Fix this
	return m_osc_type && m_event_buffer_format;
      }
      
    protected:
      
      void write_osc(uint32_t port, const char* path, const char* types, ...) {
	lv2_event_buffer_reset(m_buffer, 0, m_buffer->data);
	LV2_Event_Iterator iter;
	lv2_event_begin(&iter, m_buffer);
	va_list ap;
	va_start(ap, types);
	uint32_t size = lv2_osc_event_vsize(path, types, ap);
	va_end(ap);
	if (!size)
	  return;
	va_start(ap, types);
	bool success = lv2_osc_buffer_vappend(&iter, 0, 0, m_osc_type, 
					      path, types, size, ap);
	va_end(ap);
	static_cast<Derived*>(this)->
	  write(port, m_buffer->header_size + m_buffer->capacity, 
		m_event_buffer_format, m_buffer);

      }
      
      uint32_t m_osc_type;
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
    friend class WriteMIDI::I<Derived>;
    friend class WriteOSC::I<Derived>;
    
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
      // bother with it - this is threadsafe since hosts are not allowed
      // to instantiate the same plugin concurrently
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
