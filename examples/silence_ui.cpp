/*
    silence_ui.cpp - LV2 Toolkit UI Example

    Copyright (C) 2012  Michael Fisher <mfisher31@gmail.com>

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

/**
   @file silence_ui.cpp
   Demonstrates of creating a GtkUI manually without the GtkUI Mixin.
 */

#include <gtkmm.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lvtk/atom.hpp>
#include <lvtk/map.hpp>
#include <lvtk/ui.hpp>

using namespace lvtk;
using namespace Gtk;
using namespace sigc;

using Glib::ustring;


/* An atom-like struct for raw MIDI note on/offs */
struct midi_t {
    LV2_Atom atom;
    uint8_t midi[3];
};

class SilenceGtk : public ui::Instance<SilenceGtk>
{
public:
    SilenceGtk (const ui::InstanceArgs& args)
        : Instance (args)
    {
        for (const auto& f : args.features)
            if (map.set_feature (f))
                break;
        
        /* Required before creating widgets */
        Gtk::Main::init_gtkmm_internals();

        /*  Create the container with @c new because we don't
            know if the host is using Gtkmm or not */
        p_hbox = new Gtk::HBox();

        if (p_hbox) {
            Button *btn = manage (new Button (ustring ("Silence")));

            btn->signal_pressed().connect(
                    mem_fun(*this, &SilenceGtk::send_note_on));
            btn->signal_released().connect(
                    mem_fun(*this, &SilenceGtk::send_note_off));

            p_hbox->pack_start (*btn);
        }
    }

    LV2UI_Widget get_widget()
    {
        if (p_hbox) {
            return (LV2UI_Widget) p_hbox->gobj();
        }

        return nullptr;
    }

protected:
    lvtk::Map map;
    void send_note_on()
    {
        LV2_URID xfer = map(LV2_ATOM__eventTransfer);
        LV2_URID midiEvent = map(LV2_MIDI__MidiEvent);

        midi_t midi = {{3, midiEvent}, { 0x90, 0x40, 0x40 }};
        write (0, sizeof(midi), xfer, (void*)&midi);
    }

    void send_note_off()
    {
        LV2_URID xfer = map (LV2_ATOM__eventTransfer);
        LV2_URID midiEvent = map (LV2_MIDI__MidiEvent);

        midi_t midi = {{3, midiEvent},{ 0x80, 0x40, 0x40 }};
        write (0, sizeof(midi), xfer, (void*)&midi);
    }

private:
    Gtk::HBox *p_hbox = nullptr;
};

static const ui::UI<SilenceGtk> silence_gtk ("http://lvtoolkit.org/plugins/silence#ui");
