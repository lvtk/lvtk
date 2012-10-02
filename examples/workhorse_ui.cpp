/*
 	workhorse_ui.cpp - LV2 Toolkit UI Example

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

#include <lvtk/gtkui.hpp>
#include <lvtk/urid.hpp>

#include "workhorse.h"

using namespace lvtk;
using namespace Gtk;

using Glib::ustring;

/* The GtkUI mixin is added to UI below.  It implements the
   widget() method for you, initializes gtkmm, and provides
   an add() method for the main widget.

   You aren't required, but highly recommended, to create
   your main widget in the UI's ctor. */

class WorkhorseGtk : public UI<WorkhorseGtk, GtkUI<true>, URID<true> >
{
public:

	WorkhorseGtk (const char* plugin_uri)
	{
		/* Create the main widget. This isn't functional. Instead
		   of a plain Gtk::Button, you can add any Gtk::Widget
		   as the Main Widget */

		Button *btn = manage (new Button(ustring("Workhorse")));

		/* Add the main widget */
		add (*btn);
	}

};

int _ = WorkhorseGtk::register_class ("http://lvtoolkit.org/plugins/workhorse#ui");
