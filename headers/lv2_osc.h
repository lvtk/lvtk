/* LV2 OSC Messages Extension
 * Copyright (C) 2007 Dave Robillard <dave@drobilla.net>
 * 
 * Modified 2008 by Lars Luthman
 * 
 * This header is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This header is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef LV2_OSC_H
#define LV2_OSC_H

#include <stdarg.h>
#include <stdint.h>
#include <lv2_event_helpers.h>

/** @file
 * This is an LV2 event type specification for OSC events.
 * Additional (ie beyond raw OSC) indexing information is stored in the buffer
 * for performance, so that accessors for messages and arguments are very fast:
 * O(1) and realtime safe, unless otherwise noted.
 *
 * Everything in here is EXPERIMENTAL and UNSUPPORTED. Don't use any of it.
 */


/** Argument (in a message).
 *
 * The name of the element in this union directly corresponds to the OSC
 * type tag character in LV2_Event::types.
 */
typedef union {
	/* Standard OSC types */
	int32_t i; /**< 32 bit signed integer */
	float   f; /**< 32 bit IEEE-754 floating point number ("float") */
	char    s; /**< Standard C, NULL terminated string */
	uint8_t b; /**< Blob (int32 size then size bytes padded to 32 bits) */
	
	/* "Nonstandard" OSC types (defined in the OSC standard) */
	int64_t h; /* 64 bit signed integer */
	// t       /* OSC-timetag */
	double  d; /* 64 bit IEEE 754 floating point number ("double") */
	// S       /* Symbol, represented as an OSC-string */
	int32_t c; /* Character, represented as a 32-bit integer */
	// r  /* 32 bit RGBA color */
	// m  /* 4 byte MIDI message. Bytes from MSB to LSB are: port id, status byte, data1, data2 */
	// T  /* True. No bytes are allocated in the argument data. */
	// F  /* False. No bytes are allocated in the argument data. */
	// N  /* Nil. No bytes are allocated in the argument data. */
	// I  /* Infinitum. No bytes are allocated in the argument data. */
	// [  /* The beginning of an array. */
	// ]  /* The end of an array. */
} LV2_OSC_Argument;



/** Message.
 *
 * This is an OSC message at heart, but with some additional cache information
 * to allow fast access to parameters.  This is the payload of an LV2_Event,
 * time stamp and size (being generic) are in the containing header.
 */
typedef struct {
	uint32_t data_size;      /**< Total size of data, in bytes */
	uint32_t argument_count; /**< Number of arguments in data */
	uint32_t types_offset;   /**< Offset of types string in data */

	/** Take the address of this member to get a pointer to the remaining data.
	 * 
	 * Contents are an argument index:
	 * uint32_t argument_index[argument_count]
	 *
	 * followed by a standard OSC message:
	 * char     path[path_length]     (padded OSC string)
	 * char     types[argument_count] (padded OSC string)
	 * void     data[data_size]
	 */
	char data;

} LV2_OSC_Event;

static inline uint32_t lv2_osc_get_osc_message_size(const LV2_OSC_Event* msg)
	{ return (msg->argument_count * sizeof(char) + 1) + msg->data_size; }

static inline const void* lv2_osc_get_osc_message(const LV2_OSC_Event* msg)
	{ return (const void*)(&msg->data + (sizeof(uint32_t) * msg->argument_count)); }

static inline const char* lv2_osc_get_path(const LV2_OSC_Event* msg)
	{ return (const char*)(&msg->data + (sizeof(uint32_t) * msg->argument_count)); }

static inline const char* lv2_osc_get_types(const LV2_OSC_Event* msg)
	{ return (const char*)(&msg->data + msg->types_offset); }

static inline LV2_OSC_Argument* lv2_osc_get_argument(const LV2_OSC_Event* msg, uint32_t i)
	{ return (LV2_OSC_Argument*)(&msg->data + ((uint32_t*)&msg->data)[i]); }

static inline uint32_t lv2_osc_pad_size(uint32_t size) { 
  return size + 4 - (size % 4 ? size % 4 : 4);
}

/** Returns the size required for the given event or 0 if the 
    event is invalid. */
static inline uint32_t lv2_osc_event_vsize(const char* path, 
					   const char* types, va_list ap) {
  
  /* First, compute the size needed for the event */
  
  uint32_t n_args = strlen(types);
  uint32_t path_length = strlen(path);
  uint32_t event_size = 3 * sizeof(uint32_t);
  event_size += n_args * sizeof(uint32_t);
  event_size += lv2_osc_pad_size(path_length + 1);
  event_size += lv2_osc_pad_size(n_args + 1);
  
  const char* sptr;
  const uint8_t* bptr;
  int32_t blob_size;
  
  for (uint32_t i = 0; i < n_args; ++i) {
    switch (types[i]) {
    case 'i': 
      va_arg(ap, int32_t); 
      event_size += sizeof(int32_t); 
      break;
    case 'f': 
      va_arg(ap, double); 
      event_size += sizeof(float); 
      break;
    case 's': 
      sptr = va_arg(ap, const char*);
      event_size += lv2_osc_pad_size(strlen(sptr) + 1);
      break;
    case 'b':
      bptr = va_arg(ap, const uint8_t*);
      blob_size = *(const int32_t*)bptr + sizeof(int32_t);
      event_size += lv2_osc_pad_size(blob_size);
      break;
    default: /* Unknown data type */
      return 0;
    }
  }
  
  return event_size;
}


/** Appends the given OSC event to the event buffer. Assumes that there
    is enough space. */
static inline bool lv2_osc_buffer_vappend(LV2_Event_Iterator* iter,
					  uint32_t frames, uint32_t subframes,
					  uint16_t osc_type, const char* path, 
					  const char* types, uint32_t size,
					  va_list ap) {
  
  /* Reserve the needed space */
  uint8_t* event_buffer = lv2_event_reserve(iter, frames, subframes, 
					    osc_type, size);
  if (!event_buffer)
    return false;
  
  LV2_OSC_Event* osc_event = (LV2_OSC_Event*)event_buffer;
  uint32_t n_args = strlen(types);
  uint32_t path_length = strlen(path);
  osc_event->data_size = size - 3 * sizeof(uint32_t);
  osc_event->argument_count = n_args;
  const char* sptr;
  const uint8_t* bptr;
  int32_t blob_size;
  
  /* Iterate over the parameters and copy them into the buffer */
  uint32_t event_size = 3 * sizeof(uint32_t);
  uint32_t* arg_offset = (uint32_t*)(event_buffer + event_size);
  event_size += n_args * sizeof(uint32_t);
  memcpy(event_buffer + event_size, path, path_length);
  memset(event_buffer + event_size + path_length, 0, 
	 lv2_osc_pad_size(path_length + 1) - path_length);
  event_size += lv2_osc_pad_size(path_length + 1);

  osc_event->types_offset = event_size - 3 * sizeof(uint32_t);
  memcpy(event_buffer + event_size, types, n_args);
  memset(event_buffer + event_size + n_args, 0, 
	 lv2_osc_pad_size(n_args + 1) - n_args);
  event_size += lv2_osc_pad_size(n_args + 1);

  for (uint32_t i = 0; i < n_args; ++i) {
    
    arg_offset[i] = event_size - 3 * sizeof(uint32_t);

    switch (types[i]) {
    case 'i': 
      *(int32_t*)(event_buffer + event_size) = va_arg(ap, int32_t); 
      event_size += sizeof(int32_t);
      break;
    case 'f': 
      *(float*)(event_buffer + event_size) = va_arg(ap, double); 
      event_size += sizeof(float); 
      break;
    case 's': 
      sptr = va_arg(ap, const char*);
      memcpy(event_buffer + event_size, sptr, strlen(sptr));
      memset(event_buffer + event_size + strlen(sptr), 0, 
	     lv2_osc_pad_size(strlen(sptr) + 1) - strlen(sptr));
      event_size += lv2_osc_pad_size(strlen(sptr) + 1);
      break;
    case 'b':
      bptr = va_arg(ap, const uint8_t*);
      blob_size = *(const int32_t*)bptr + sizeof(int32_t);
      memcpy(event_buffer + event_size, bptr, blob_size);
      memset(event_buffer + event_size + blob_size, 0,
	     lv2_osc_pad_size(blob_size) - blob_size);
      event_size += lv2_osc_pad_size(blob_size);
      break;
    default: /* Unknown data type */
      return false;
    }

  }
  
  return true;
}


static inline bool lv2_osc_buffer_append(LV2_Event_Iterator* iter,
					 uint32_t frames, uint32_t subframes,
					 uint16_t osc_type, const char* path, 
					 const char* types, ...) {
  
  va_list ap;
  va_start(ap, types);
  uint32_t size = lv2_osc_event_vsize(path, types, ap);
  va_end(ap);
  if (!size)
    return false;
  va_start(ap, types);
  bool success = lv2_osc_buffer_vappend(iter, frames, subframes, 
					osc_type, path, types, size, ap);
  va_end(ap);
  return success;
}




#endif /* LV2_OSC_H */
