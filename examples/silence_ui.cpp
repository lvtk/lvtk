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

using namespace lvtk;
using namespace Gtk;

using Glib::ustring;


// The Silence UI- Notice GtkUI<true> is not added here. The
// required widget() method impl is done manually without
// a toolkit mixin

class SilenceGtk : public UI<SilenceGtk, URID<true> >
{
public:

	SilenceGtk (const char* plugin_uri)
	{
		/* Required before creating widgets */
		Gtk::Main::init_gtkmm_internals();

		/* Create the container with @c new because we don't
		   know if the host is using Gtkmm or not */
		p_hbox = new Gtk::HBox();

		if (p_hbox) {
			Button *btn = manage (new Button(ustring("Silence")));
			p_hbox->pack_start (*btn);
		} else {
			p_hbox = 0;
		}
	}

	/*  Required implementation required by the UI class.
	    The UI library will ALWAYS try to execute
	    a @c widget() method during instantiation. The
	    toolkit mixins this. Since we aren't using a mixin,
	    we have to do it ourselves.

	    This is really easy for gtkmm. However, you can
	    return ANY pointer here as the host supports the
	    'widget type' you are returning. */

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

private:
	Gtk::HBox *p_hbox;

};

int _ = SilenceGtk::register_class ("http://lvtoolkit.org/plugins/silence#ui");
