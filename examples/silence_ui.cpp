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
   Demonstrates creating a GtkUI manually without the GtkUI Mixin.
 */

#include <gtkmm.h>
#include <lvtk/ui.hpp>

#include "silence.h"

using namespace lvtk;
using namespace Gtk;
using namespace sigc;

using Glib::ustring;


/* An atom-like struct for raw MIDI note on/offs */
struct midi_t {
    LV2_Atom atom;
    uint8_t midi[3];
};


/* The Silence UI- Notice GtkUI<true> is not added here. The
   required widget() method impl is done manually without
   a toolkit mixin */
class SilenceGtk : public UI<SilenceGtk, URID<true> >
{
public:

    SilenceGtk (const char* plugin_uri)
    {
        /*  Required before creating widgets */
        Gtk::Main::init_gtkmm_internals();

        /*  Create the container with @c new because we don't
            know if the host is using Gtkmm or not */
        p_hbox = new Gtk::HBox();

        if (p_hbox) {
            Button *btn = manage (new Button(ustring("Silence")));

            btn->signal_pressed().connect(
                    mem_fun(*this, &SilenceGtk::send_note_on));
            btn->signal_released().connect(
                    mem_fun(*this, &SilenceGtk::send_note_off));

            p_hbox->pack_start (*btn);
        } else {
            p_hbox = 0;
        }

    }


    /*  Required implementation by the UI class. The UI library will ALWAYS
        try to execute a @c widget() method during instantiation. The
        toolkit mixins implement this method. Since we aren't using a mixin,
        we have to do it ourselves.

        This is really easy for gtkmm. However, you can return ANY pointer here
        as the host supports the 'widget type' you are returning. */
    LV2UI_Widget* widget()
    {
        /* If the HBox is NULL at this point, it
		   will prevent the UI from instantiating */

        if (p_hbox) {
            /* LV2 GtkUI expects a Gtk C Object.
			   cast one to an LV2UI_Widget */
            return widget_cast (p_hbox->gobj());
        }

        return NULL;
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

private:
    /* The container object */
    Gtk::HBox *p_hbox;

};

int _ = SilenceGtk::register_class ("http://lvtoolkit.org/plugins/silence#ui");
