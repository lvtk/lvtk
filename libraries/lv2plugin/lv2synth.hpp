/****************************************************************************
    
    lv2plugin.hpp - support file for writing LV2 plugins in C++
    
    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef LV2SYNTH_HPP
#define LV2SYNTH_HPP

#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>

#include <lv2plugin.hpp>
#include <lv2-midifunctions.h>


namespace LV2 {
  
  static const unsigned char INVALID_KEY = 255;
  
  
  float key2hz(unsigned char key) {
    return 8.1758 * std::pow(1.0594, key);
  }
  

  class Voice {
  public:

    void set_port_buffers(std::vector<void*>& ports) { m_ports = &ports; }

  protected:
  
    template <typename T> inline T*& p(uint32_t port) {
      return reinterpret_cast<T*&>((*m_ports)[port]);
    }
  
    float*& p(uint32_t port) {
      return reinterpret_cast<float*&>((*m_ports)[port]);
    }
  
    std::vector<void*>* m_ports;
  };
  
  
  template <class V>
  class Synth : public Plugin {
  public:
    
    Synth(uint32_t ports, uint32_t midi_input) 
      : Plugin(ports),
	m_midi_input(midi_input) { 
    
    }
    
    
    unsigned find_free_voice(unsigned char key, unsigned char velocity) {
      for (unsigned i = 0; i < m_voices.size(); ++i) {
	if (m_voices[i]->get_key() == INVALID_KEY)
	  return i;
      }
      return 0;
    }
    
    
    void handle_midi(uint32_t size, unsigned char* data) {
      if (size != 3)
	return;
      if (data[0] == 0x90) {
	m_voices[find_free_voice(data[1], data[2])]->on(data[1], data[2]);
      }
      else if (data[0] == 0x80) {
	for (unsigned i = 0; i < m_voices.size(); ++i) {
	  if (m_voices[i]->get_key() == data[1]) {
	    m_voices[i]->off(data[2]);
	    break;
	  }
	}
      }
    }
    
    
    void run(uint32_t sample_count) {
      
      // zero output buffers
      for (unsigned i = 0; i < m_audio_ports.size(); ++i)
	std::memset(p(m_audio_ports[i]), 0, sizeof(float) * sample_count);
      
      // prepare voices
      for (unsigned i = 0; i < m_voices.size(); ++i)
	m_voices[i]->set_port_buffers(m_ports);
      
      LV2_MIDIState ms = { p<LV2_MIDI>(m_midi_input), sample_count, 0 };
      double event_time;
      uint32_t event_size;
      unsigned char* event_data;
      uint32_t samples_done = 0;
      
      while (samples_done < sample_count) {
	lv2midi_get_event(&ms, &event_time, &event_size, &event_data);
	lv2midi_step(&ms);
	handle_midi(event_size, event_data);
	uint32_t to = event_time;
	if (to > samples_done) {
	  for (unsigned i = 0; i < m_voices.size(); ++i)
	    m_voices[i]->render(samples_done, to);
	  samples_done = to;
	}
      }
      
    }
    
    void add_audio_ports(uint32_t p1 = -1, uint32_t p2 = -1, uint32_t p3 = -1,
			 uint32_t p4 = -1, uint32_t p5 = -1, uint32_t p6 = -1) {
      if (p1 == uint32_t(-1))
	return;
      m_audio_ports.push_back(p1);
      if (p2 == uint32_t(-1))
	return;
      m_audio_ports.push_back(p2);
      if (p3 == uint32_t(-1))
	return;
      m_audio_ports.push_back(p3);
      if (p4 == uint32_t(-1))
	return;
      m_audio_ports.push_back(p4);
      if (p5 == uint32_t(-1))
	return;
      m_audio_ports.push_back(p5);
      if (p6 == uint32_t(-1))
	return;
      m_audio_ports.push_back(p6);
    }
    
  protected:
    
    std::vector<V*> m_voices;
    std::vector<uint32_t> m_audio_ports;
    uint32_t m_midi_input;
    
  };
  
}


#endif
