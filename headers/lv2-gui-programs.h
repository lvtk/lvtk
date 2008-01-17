/************************************************************************
 *
 * Program management extension for LV2 GUIs
 *
 * Copyright (C) 2007-2008 Lars Luthman <lars.luthman@gmail.com>
 * 
 * This header is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This header is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 * USA.
 *
 ***********************************************************************/

#ifndef LV2_GUI_PROGRAMS_H
#define LV2_GUI_PROGRAMS_H

#include <lv2-ui.h>

/** This is a LV2 GUI Feature (not a LV2 Feature) that allows a LV2 GUI
    host to tell a plugin GUI about existing presets for the plugin, and
    a plugin GUI to request switching to one of those presets or saving
    the current state as a new preset. It does only specify the way GUIs
    and hosts communicate about plugin presets, not how or where these
    presets are actually stored - that is up to the host.
    
    Each preset has a name and a numeric identifier. The numeric identifier
    is a uint32_t, the 7 least significant bits of which should be considered
    the "program number" and the rest, shifted 7 bits to the right, the "bank
    number" to correspond to MIDI programs. The full 25 bit range of the bank
    number may be used, but remember that MIDI only has 14 bit bank numbers
    (7 bit coarse + 7 bit fine) so anything above that will not be addressable
    by MIDI program changes.
    
    The special numeric identifier 4294967295 = 2^32 - 1 (all bits set) may 
    only be used in the plugin callback current_program_changed() to indicate 
    that there is no currently active program.
*/

/** The host descriptor. A pointer to one of these should be passed to the 
    GUI's instantiate() callback in a LV2_Feature with the URI 
    http://ll-plugins.nongnu.org/lv2/ext/ui#ext_programs.
*/
typedef struct {
  
  /** This is the host-provided function that the GUI can use to request a 
      program change in the host. Calling this function does not guarantee 
      that the program will change, it is just a request. If the program 
      does change the GUI's current_program_changed() callback will be 
      called, either before or after this function returns depending on 
      whether the GUI host <-> plugin instance communication is
      synchronous or asynchronous.
      
      This member must not be set to NULL.
  */
  void (*change_program)(LV2UI_Controller controller,
			 uint32_t         program);

  /** This is the host-provided function that the GUI can use to request 
      that the current state of the plugin is saved to a program. Calling 
      this function does not guarantee that the state will be saved, it is
      just a request. If the state is saved the GUI's program_added() 
      callback will be called, either before or after this function returns 
      depending on whether the GUI host <-> plugin instance communication
      is synchronous or asynchronous.
      
      This member must not be set to NULL.
  */
  
  void (*save_program)(LV2UI_Controller controller,
		       uint32_t         program,
		       const char*      name);
  
} LV2UI_Programs_HDesc;


/** The GUI descriptor. One of these should be returned by the GUI's
    extension_data() callback when called with the URI
    http://ll-plugins.nongnu.org/lv2/ext/ui#ext_programs.
*/    
typedef struct {
  
  /** This function is called when the host adds a new program to its program
      list, or changes the name of an old one. It may be set to NULL if the 
      GUI isn't interested in displaying program information.
  */
  void (*program_added)(LV2UI_Handle gui, 
                        uint32_t     number, 
			const char*  name);
  
  /** This function is called when the host removes a program from its program
      list. It may be set to NULL if the GUI isn't interested in displaying
      program information.
      
      The number parameter should always be a number for a program that has 
      been added previously for this GUI instance.
  */
  void (*program_removed)(LV2UI_Handle gui, 
                          uint32_t     number);
  
  /** This function is called when the host clears its program list. It may be
      set to NULL if the GUI isn't interested in displaying program 
      information. 
  */
  void (*programs_cleared)(LV2UI_Handle gui);
  
  /** This function is called when the host changes the current program. It may
      be set to NULL if the GUI isn't interested in displaying program 
      information.
      
      The number parameter should always be the number of a program that has 
      been added previously for this GUI instance, or 2^32 - 1 to indicate 
      that there is no currently active preset.
  */
  void (*current_program_changed)(LV2UI_Handle gui, 
                                  uint32_t     number);

} LV2UI_Programs_GDesc;


#endif
