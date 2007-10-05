/************************************************************************

   This file is partly based on dssi.h from DSSI 0.9, which is
   Copyright (C) 2004 Chris Cannam, Steve Harris and Sean Bolton

   Modifications (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.

*************************************************************************/

#ifndef LV2_COMMAND_H
#define LV2_COMMAND_H

#include "lv2.h"

#ifdef __cplusplus
extern "C" {
#endif


/* 
   XXX more docs here - function class etc

   This header defines data structures used by LV2 hosts and plugins that
   support the LV2 Instrument extension with URI 
   <http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension>.
   
   This extension adds a callback that can be used to send arbitrary
   commands to a plugin instance, and a callback that the plugin can call
   (in its implementation of the other callback) to send feedback to the host
   (and through it, to a GUI).

   A plugin that supports this extension must have a non-NULL extension_data()
   function pointer in its descriptor, that returns a pointer to an 
   LV2_CommandDescriptor, defined below, when called with the URI 
   "http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension". This 
   LV2_CommandDescriptor object contains a function pointer.
*/



/** This struct contains all plugin callbacks used in this extension. */
typedef struct {
  
  char* (*command)(LV2_Handle         instance,
		   uint32_t           argc,
		   const char* const* argv);
  
} LV2_CommandDescriptor;


/** This struct contains all host callbacks used in this extension. */
typedef struct {
  
  void (*feedback)(void*                host_data,
		   uint32_t             argc,
		   const char* const*   argv);
  void* host_data;

} LV2_CommandHostDescriptor;
     
#ifdef __cplusplus
}
#endif


#endif
