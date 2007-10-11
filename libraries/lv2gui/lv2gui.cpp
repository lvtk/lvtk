/****************************************************************************
    
    lv2gui.cpp - Wrapper library to make it easier to write LV2 GUIs in C++
    
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

#include <cstring>
#include <iostream>

#include "lv2gui.hpp"


using namespace std;


namespace LV2 {


  Controller::Controller()
    : m_wfunc(0),
      m_cfunc(0),
      m_ctrl(0) {
    
  }
  
  
  void Controller::write(uint32_t port, uint32_t buffer_size, 
			 const void* buffer) {
    if (m_wfunc)
      m_wfunc(m_ctrl, port, buffer_size, buffer);
  }
  
  
  void Controller::command(uint32_t argc, const char* const* argv) {
    
    cerr<<__PRETTY_FUNCTION__<<endl;
    cerr<<"POINTERS:"<<endl;
    for (unsigned i = 0; i < argc; ++i)
      cerr<<"  "<<(const void*)(argv[i])<<endl;
    
    if (m_cfunc)
      m_cfunc(m_ctrl, argc, argv);
  }
  
  
  void Controller::request_program(unsigned char number) {
    if (m_pfunc)
      m_pfunc(m_ctrl, number);
  }
  

  void Controller::request_save(unsigned char number, const char* name) {
    if (m_sfunc)
      m_sfunc(m_ctrl, number, name);
  }
  


  Controller::Controller(LV2UI_Write_Function wfunc, 
			 LV2UI_Command_Function cfunc,
			 LV2UI_Program_Change_Function pfunc,
			 LV2UI_Program_Save_Function sfunc,
			 LV2UI_Controller ctrl,
			 const LV2_Feature* const* features)
    : m_wfunc(wfunc),
      m_cfunc(cfunc),
      m_pfunc(pfunc),
      m_sfunc(sfunc),
      m_ctrl(ctrl) {
    
  }
  
  
  GUI::DescList& GUI::get_lv2g2g_descriptors() {
    static DescList list;
    return list;
  }
  
  
  void GUI::delete_ui_instance(LV2UI_Handle instance) {
    delete static_cast<GUI*>(instance);
  }
  
    
  void GUI::port_event(LV2UI_Handle instance, uint32_t port, uint32_t buffer_size,
		       const void* buffer) {
    static_cast<GUI*>(instance)->port_event(port, buffer_size, buffer);
  }
  
    
  void GUI::feedback(LV2UI_Handle instance, uint32_t argc, const char* const* argv) {
    static_cast<GUI*>(instance)->feedback(argc, argv);
  }
    
    
  void GUI::program_added(LV2UI_Handle instance, unsigned char number, 
			  const char* name) {
    static_cast<GUI*>(instance)->program_added(number, name);
  }
    
    
  void GUI::program_removed(LV2UI_Handle instance, unsigned char number) {
    static_cast<GUI*>(instance)->program_removed(number);
  }
  
  
  void GUI::programs_cleared(LV2UI_Handle instance) {
    static_cast<GUI*>(instance)->programs_cleared();
  }
  
  
  void GUI::current_program_changed(LV2UI_Handle instance, unsigned char number) {
    static_cast<GUI*>(instance)->current_program_changed(number);
  }
  
  
  void* GUI::extension_data(LV2UI_Handle instance, const char* URI) {
    return static_cast<GUI*>(instance)->extension_data(URI);
  }
  

}


extern "C" {
  
  const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index) {
    LV2::GUI::DescList descs = LV2::GUI::get_lv2g2g_descriptors();
    if (index >= descs.size())
      return 0;
    return descs[index];
  }
  
}


