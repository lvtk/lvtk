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

#ifndef LV2_UI_COMMAND_H
#define LV2_UI_COMMAND_H

#include "lv2-ui.h"

#ifdef __cplusplus
extern "C" {
#endif


/* 
   XXX more docs here
*/



/** This struct contains all host callbacks used in this extension. */
typedef struct {
  
  void (*command)(LV2UI_Handle       instance,
		  uint32_t           argc,
		  const char* const* argv);
  
} LV2UI_Command_HDesc;


/** This struct contains all UI callbacks used in this extension. */
typedef struct {
  
  void (*feedback)(LV2UI_Controller     controller,
		   uint32_t             argc,
		   const char* const*   argv);

} LV2UI_Command_GDesc;

     
#ifdef __cplusplus
}
#endif


#endif
