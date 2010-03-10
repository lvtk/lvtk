/************************************************************************
 *
 * Preset management extension for LV2 UIs
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

#ifndef LV2_UI_PRESETS_H
#define LV2_UI_PRESETS_H

#include <lv2_ui.h>

/** This is a LV2 UI Feature (not a LV2 Feature) that allows a LV2 UI
    host to tell a plugin UI about existing presets for the plugin, and
    a plugin UI to request switching to one of those presets or saving
    the current state as a new preset. It does only specify the way UIs
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
    only be used in the plugin callback current_preset_changed() to indicate 
    that there is no currently active preset.
*/


#define LV2_UI_PRESETS_URI "http://lv2plug.in/ns/extensions/ui#ext_presets"

#define LV2_UI_PRESETS_NOPRESET 4294967295


/** The host descriptor. A pointer to one of these should be passed to the 
    UI's instantiate() callback in a LV2_Feature with the URI 
    http://lv2plug.in/ns/extensions/ui#ext_presets.
*/
typedef struct {
  
  /** This is the host-provided function that the UI can use to request a 
      preset change in the host. Calling this function does not guarantee 
      that the preset will change, it is just a request. If the preset 
      does change the UI's current_preset_changed() callback will be 
      called, either before or after this function returns depending on 
      whether the UI host <-> plugin instance communication is
      synchronous or asynchronous.
      
      This member must not be set to NULL.
  */
  void (*change_preset)(LV2UI_Controller controller,
			uint32_t         preset);

  /** This is the host-provided function that the UI can use to request 
      that the current state of the plugin is saved to a preset. Calling 
      this function does not guarantee that the state will be saved, it is
      just a request. If the state is saved the UI's preset_added() 
      callback will be called, either before or after this function returns 
      depending on whether the UI host <-> plugin instance communication
      is synchronous or asynchronous.
      
      This member must not be set to NULL.
  */
  
  void (*save_preset)(LV2UI_Controller controller,
		      uint32_t         preset,
		      const char*      name);
  
} LV2UI_Presets_Feature;


/** The UI descriptor. One of these should be returned by the UI's
    extension_data() callback when called with the URI
    http://lv2plug.in/ns/extensions/ui#ext_presets.
*/    
typedef struct {
  
  /** This function is called when the host adds a new preset to its preset
      list, or changes the name of an old one. It may be set to NULL if the 
      UI isn't interested in displaying the preset list.
  */
  void (*preset_added)(LV2UI_Handle ui, 
		       uint32_t     number, 
		       const char*  name);
  
  /** This function is called when the host removes a preset from its preset
      list. It may be set to NULL if the UI isn't interested in displaying
      the preset list.
      
      The number parameter should always be a number for a preset that has 
      been added previously for this UI instance.
  */
  void (*preset_removed)(LV2UI_Handle ui, 
			 uint32_t     number);
  
  /** This function is called when the host clears its preset list. It may be
      set to NULL if the UI isn't interested in displaying the preset list. 
  */
  void (*presets_cleared)(LV2UI_Handle ui);
  
  /** This function is called when the host changes the current preset. It may
      be set to NULL if the UI isn't interested in displaying the current 
      preset.
      
      The number parameter should always be the number of a preset that has 
      been added previously for this UI instance, or LV2_UI_PRESETS_NOPRESET
      to indicate that there is no currently active preset.
  */
  void (*current_preset_changed)(LV2UI_Handle ui, 
				 uint32_t     number);

} LV2UI_Presets_GDesc;


#endif
