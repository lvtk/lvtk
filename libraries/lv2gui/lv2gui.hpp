/****************************************************************************
    
    lv2gui.hpp - Wrapper library to make it easier to write LV2 GUIs in C++
    
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef LV2GUI_HPP
#define LV2GUI_HPP

#include <iostream>
#include <map>
#include <string>

#include <gtkmm/box.h>
#include <gtkmm/main.h>
#include <gtkmm/widget.h>

#include <lv2-gui.h>
#include <lv2types.hpp>


namespace LV2 {


  typedef std::vector<LV2UI_Descriptor*> GUIDescList;
    
  
  /** @internal
      This function returns the list of LV2 descriptors. It should only be 
      used internally. */
  GUIDescList& get_lv2g2g_descriptors();

  
  /** @internal
      This class contains no data, it's just used to make it impossible to
      create copies of instances of subclasses.  */
  class NonCopyable {
    NonCopyable(const NonCopyable&) { }
    const NonCopyable& operator=(const NonCopyable&) { }
  };
  
  
  /** This class is an interface for controlling a plugin instance. An object
      of this class will be provided by the host when a plugin GUI is 
      instantiated. */
  template<class Derived,
           class Ext1 = Empty, class Ext2 = Empty, class Ext3 = Empty,
           class Ext4 = Empty, class Ext5 = Empty, class Ext6 = Empty,
           class Ext7 = Empty, class Ext8 = Empty, class Ext9 = Empty>
  class Controller : public SimpleMixinTree<Derived, Ext1, Ext2, Ext3, Ext4, 
					    Ext5, Ext6, Ext7, Ext8, Ext9>, 
		     private NonCopyable {
  public:
    
    /** @internal
	This is only used by the wrapper functions. */
    Controller(LV2UI_Write_Function wfunc, 
	       LV2UI_Controller ctrl,
	       const LV2_Feature* const* features)
      : m_wfunc(wfunc),
	m_ctrl(ctrl) {
      
    }
    
    /** Write to a port in the plugin instance. */
    void write(uint32_t port, uint32_t buffer_size, const void* buffer) {
      if (m_wfunc)
	m_wfunc(m_ctrl, port, buffer_size, buffer);
    }
    
    /** Convenient wrapper for writing to control ports. */
    inline void write_control(uint32_t port, const float& value) {
      write(port, sizeof(float), &value);
    }
    
    /** Send a command to the plugin instance. */
    /*
    void command(uint32_t argc, const char* const* argv) {
      std::cerr<<__PRETTY_FUNCTION__<<std::endl;
      std::cerr<<"POINTERS:"<<std::endl;
      for (unsigned i = 0; i < argc; ++i)
	std::cerr<<"  "<<(const void*)(argv[i])<<std::endl;
      
      if (m_cfunc)
	m_cfunc(m_ctrl, argc, argv);
    }
    */
    
    /** Tell the plugin host to switch to program @c number for the plugin
	instance. */
    /*
    void request_program(unsigned char number) {
      if (m_pfunc)
	m_pfunc(m_ctrl, number);
    }
    */
    
    /** Tell the plugin host to save the current plugin state to a program. */
    /*
    void request_save(unsigned char number, const char* name) {
      if (m_sfunc)
	m_sfunc(m_ctrl, number, name);
    }
    */
    
  protected:
    
    LV2UI_Write_Function m_wfunc;
    LV2UI_Controller m_ctrl;

  };
  

  /** This is the base class for a plugin GUI. You should inherit it and 
      override any functions you want to provide implementations for. 
      All subclasses must have a constructor with the signature
      
      MyGUI(LV2::Controller& controller, const char* plugin_URI, 
            const char* bundle_path, Gtk::Widget*& UI_widget);
	    
      Where @c controller is the interface for controlling the plugin instance
      and @c UI_widget should be set to point to a widget that the host will
      display to the user. The plugin is responsible for deallocating the 
      widget when the destructor for the LV2GUI subclass is called. */
  template<class Derived,
           template <class, bool> class Ext1 = End, bool Req1 = false,
           template <class, bool> class Ext2 = End, bool Req2 = false,
           template <class, bool> class Ext3 = End, bool Req3 = false,
           template <class, bool> class Ext4 = End, bool Req4 = false,
           template <class, bool> class Ext5 = End, bool Req5 = false,
           template <class, bool> class Ext6 = End, bool Req6 = false,
           template <class, bool> class Ext7 = End, bool Req7 = false,
           template <class, bool> class Ext8 = End, bool Req8 = false,
           template <class, bool> class Ext9 = End, bool Req9 = false>
  class GUI : public Gtk::HBox, public MixinTree<Derived, 
						 Ext1, Req1, Ext2, Req2,
						 Ext3, Req3, Ext4, Req4, 
						 Ext5, Req5, Ext6, Req6,
						 Ext7, Req7, Ext8, Req8, 
						 Ext9, Req9> {
  public:

    typedef Controller<Derived, 
		       typename Ext1<Derived, false>::C, 
		       typename Ext2<Derived, false>::C,
		       typename Ext3<Derived, false>::C, 
		       typename Ext4<Derived, false>::C, 
		       typename Ext5<Derived, false>::C, 
		       typename Ext6<Derived, false>::C, 
		       typename Ext7<Derived, false>::C, 
		       typename Ext8<Derived, false>::C,
		       typename Ext9<Derived, false>::C> 
    Controller;

    
    ~GUI() { delete m_controller; }
    
    /** Override this if you want your GUI to do something when a control port
	value changes in the plugin instance. */
    void port_event(uint32_t port, uint32_t buffer_size, const void* buffer) { }
    
    /** Use this template function to register a class as a LV2 GUI. */
    static int register_class(const std::string& URI) {
      LV2UI_Descriptor* desc = new LV2UI_Descriptor;
      std::memset(desc, 0, sizeof(LV2UI_Descriptor));
      desc->URI = strdup(URI.c_str());
      desc->instantiate = &create_ui_instance;
      desc->cleanup = &delete_ui_instance;
      desc->port_event = &port_event;
      desc->feedback = &feedback;
      desc->program_added = &program_added;
      desc->program_removed = &program_removed;
      desc->programs_cleared = &programs_cleared;
      desc->current_program_changed = &current_program_changed;
      desc->extension_data = &extension_data;
      get_lv2g2g_descriptors().push_back(desc);
      return get_lv2g2g_descriptors().size() - 1;
    }
    
  private:
    
    /** @internal
	This function creates an instance of a plugin GUI. It is used 
	as the instantiate() callback in the LV2 descriptor. You should not use
	it directly. */
    static LV2UI_Handle create_ui_instance(const struct _LV2UI_Descriptor* 
					   descriptor,
					   const char* plugin_uri,
					   const char* bundle_path,
					   LV2UI_Write_Function write_func,
					   LV2UI_Controller ctrl,
					   LV2UI_Widget* widget,
					   const LV2_Feature* const* features) {
      
      // this is needed to initialise gtkmm stuff in case we're running in
      // a Gtk+ or PyGtk host or some other language
      Gtk::Main::init_gtkmm_internals();
      
      // create a Controller object that wraps all the host callbacks
      Controller* controller = new Controller(write_func, ctrl, features);
      
      // create the GUI object
      Derived* t = new Derived(*controller, plugin_uri, bundle_path);
      
      // the GUI object assumes ownership of the Controller pointer and will delete
      // it in its destructor
      t->m_controller = controller;
      
      *widget = static_cast<Gtk::Widget*>(t)->gobj();
      return reinterpret_cast<LV2UI_Handle>(t);
    }
    

    /** @internal
	This function destroys an instance of a GUI. It is used as the
	cleanup() callback in the LV2UI descriptor. You should not use it directly. */
    static void delete_ui_instance(LV2UI_Handle instance) {
      delete static_cast<Derived*>(instance);
    }
    
    static void port_event(LV2UI_Handle instance, uint32_t port, 
			   uint32_t buffer_size, const void* buffer) {
      static_cast<Derived*>(instance)->port_event(port, buffer_size, buffer);
    }
    
    static void feedback(LV2UI_Handle instance, uint32_t argc, 
			 const char* const* argv) {
      static_cast<Derived*>(instance)->feedback(argc, argv);
    }
    
    static void program_added(LV2UI_Handle instance, unsigned char number, 
			      const char* name) {
      static_cast<Derived*>(instance)->program_added(number, name);
    }
    
    static void program_removed(LV2UI_Handle instance, unsigned char number) {
      static_cast<Derived*>(instance)->program_removed(number);
    }
    
    static void programs_cleared(LV2UI_Handle instance) {
      static_cast<Derived*>(instance)->programs_cleared();
    }
    
    static void current_program_changed(LV2UI_Handle instance, unsigned char number) {
      static_cast<Derived*>(instance)->current_program_changed(number);
    }
    
    static void* extension_data(LV2UI_Handle instance, const char* URI) {
      return static_cast<GUI*>(instance)->extension_data(URI);
    }
    
    
    Controller* m_controller;
    
  };
  

  /** Base class for GUI extensions. GUI extension mixin classes don't have to 
      inherit from this class, but it's convenient. */
  template <bool Required>
  struct GUIExtension : public Extension<Required> {
    
    typedef Empty C;
    
  };

  
}


#endif
