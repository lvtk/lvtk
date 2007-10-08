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
#include <iostream>
#include <vector>

#include <lv2plugin.hpp>
#include <lv2-midifunctions.h>


namespace LV2 {
  

  class Voice {
  public:
    void on(unsigned char key, unsigned char velocity) { };
    void off(unsigned char velocity) { };
    void fast_off() { };
    unsigned char get_key() { return 255; }
  };
  
  
  template <class V>
  class Synth : public Plugin {
  public:
    

    Synth(uint32_t ports, uint32_t midi_input) 
      : Plugin(ports), 
	m_midi_input(midi_input) { 
      
    }
    
    
    ~Synth() {
      for (unsigned i = 0; i < m_voices.size(); ++i)
	delete m_voices[i];
    }
    
    
    void render_voices(uint32_t from, uint32_t to) { }
    
    
    unsigned find_free_voice(unsigned char key) {
      unsigned i;
      for (i = 0; i < m_voices.size(); ++i) {
	if (m_voices[i]->get_key() == 255)
	  break;
      }
      if (i == m_voices.size())
	return 0;
      return i;
    }
    
    
    void run(uint32_t sample_count) { 
      
      std::cerr<<__PRETTY_FUNCTION__<<std::endl;
      
      LV2_MIDIState ms = { p<LV2_MIDI>(m_midi_input), sample_count, 0 };
      double event_time;
      uint32_t event_size;
      unsigned char* event_data;
      uint32_t samples_done = 0;
      do {
	
	lv2midi_get_event(&ms, &event_time, &event_size, &event_data);
	
	std::cerr<<"EVENT: "<<event_time<<", "<<event_size<<std::endl;
	
	lv2midi_step(&ms);
	uint32_t to(std::floor(event_time));
	if (to > samples_done) {
	  render_voices(samples_done, to);
	  samples_done = to;
	}
	
	if (samples_done < sample_count && event_size == 3) {
	  switch (event_data[0]) {
	    
	    // note off
	  case 0x80:
	    for (unsigned i = 0; i < m_voices.size(); ++i) {
	      if (m_voices[i]->get_key() == event_data[1]) {
		m_voices[i]->off(event_data[2]);
		break;
	      }
	    }
	    break;
	    
	    // note on
	  case 0x90:
	    m_voices[find_free_voice(event_data[1])]->
	      on(event_data[1], event_data[2]);
	    break;
	    
	    // controller
	  case 0xB0:
	    // all notes off
	    if (event_data[1] == 0x7B) {
	      for (unsigned i = 0; i < m_voices.size(); ++i)
		m_voices[i]->off(64);
	    }
	    // all sound off
	    if (event_data[1] == 0x78) {
	      for (unsigned i = 0; i < m_voices.size(); ++i)
		m_voices[i]->fast_off();
	    }
	    break;

	  }
	}
      } while (samples_done < sample_count);
    }
    
    
  protected:
    
    std::vector<V*> m_voices;
    int m_pwheel;
    
  private:
    
    uint32_t m_midi_input;
  };
    
}


#endif
