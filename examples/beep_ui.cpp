/*
  beep_ui.cpp  -  LV2 Toolkit API Demonstration Plugin

  Copyright (C) 2004-2010  Lars Luthman
  Updated for LVTK by Michael Fisher <mfisher31@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <lvtk/gtkui.hpp>
#include "beep.h"


using namespace sigc;
using namespace Gtk;


/* An atom-like struct for raw MIDI note on/offs */
struct midi_t {
    LV2_Atom atom;
    uint8_t midi[3];
};


class BeepGUI : public lvtk::UI<BeepGUI, lvtk::GtkUI<true>, lvtk::URID<true> > {
public:
  
    BeepGUI(const char* URI) : m_button("Click me!")
    {
        m_button.signal_pressed().connect(
                mem_fun(*this, &BeepGUI::send_note_on));
        m_button.signal_released().connect(
                mem_fun(*this, &BeepGUI::send_note_off));
        add (m_button);
    }
  
protected:
  
    /* Raw MIDI Senders */

    void send_note_on()
    {
        LV2_URID xfer = map(LV2_ATOM__eventTransfer);
        LV2_URID midiEvent = map(LV2_MIDI__MidiEvent);

        midi_t midi = {{3, midiEvent}, { 0x90, 0x40, 0x40 }};
        write (p_midi, sizeof(midi), xfer, (void*)&midi);
    }

    void send_note_off()
    {
        LV2_URID xfer = map(LV2_ATOM__eventTransfer);
        LV2_URID midiEvent = map(LV2_MIDI__MidiEvent);

        midi_t midi = {{3, midiEvent},{ 0x80, 0x40, 0x40 }};
        write (p_midi, sizeof(midi), xfer, (void*)&midi);
    }

    Button m_button;
  
};

static int _ = BeepGUI::register_class(
        "http://ll-plugins.nongnu.org/lv2/lv2pftci/beep/gui");
