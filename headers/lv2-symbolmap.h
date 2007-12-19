/****************************************************************************
    
    lv2-symbolmap.h - header file for mapping text strings to integers in
                      LV2 plugins
    
    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    
    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef LV2_SYMBOLMAP_H
#define LV2_SYMBOLMAP_H

#include <stdint.h>


/** @file
    This file contains the specification for an LV2 symbol->integer mapping
    mechanism. It allows a plugin to request integer ID numbers for arbitrary
    character arrays, for example URIs for things like event types. This can
    be useful when you want to use arbitrary strings as identifiers but want
    to be able to compare them very quickly.
    
    This header is not useful on its own, it is meant to be used by other
    LV2 extensions that need symbol -> integer mappings (for example a generic
    event port extension that uses URIs to identify event types and want to
    map them to integers to be able to compare event types quickly in the
    run() callback).
*/


/** The type of the callback data for the symbol mapping function provided
    by the host.
*/
typedef void* LV2_Symbol_Callback_Data;


/** This struct contains the information needed for a plugin to map strings
    to integers. It should be provided by the host, probably as the data 
    pointer in a LV2_Feature, but the exact mechanism is defined by the
    extension that uses it.
*/
typedef struct {
  
  /** This function maps character arrays to integers. The callback_data 
      parameter must be the callback_data member of the same struct variable
      as the function pointer, and the symbol parameter must be a '\0' 
      terminated array. It may not be a NULL pointer, but any other character
      array terminated by '\0' is allowed - there is no requirement that it
      must adhere to any specific text encoding or other format.
      
      A return value of 0 means that the symbol table is full, or that the host
      for some other reason can't or won't map that particular symbol to an
      integer. Any other value is a valid integer ID for the given symbol.
      
      This function must always return the same value when called with the same
      symbol for a given plugin instance.
  */
  uint32_t (*map_symbol)(LV2_Symbol_Callback_Data callback_data,
			 char const*              symbol);
  
  /** A data pointer that must be passed as the first parameter to 
      map_symbol() every time it is called. The plugin should not try to 
      interpret it in any way.
  */
  LV2_Symbol_Callback_Data callback_data;

} LV2_Symbol_Map;


#endif
