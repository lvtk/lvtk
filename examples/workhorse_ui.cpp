/*
 * worker_ui.cpp
 *
 *  Created on: Sep 30, 2012
 *      Author: mfisher
 */


#include <lvtk/gtkui.hpp>
#include <lvtk/urid.hpp>

using namespace lvtk;
using namespace Gtk;

using Glib::ustring;

class WorkhorseGtk : public UI<WorkhorseGtk, GtkUI<true>, URID<true> >
{
public:

	WorkhorseGtk (const char* plugin_uri)
	{
		Button *btn = manage (new Button(ustring("Workhorse")));
		add (*btn);
	}

};

int _ = WorkhorseGtk::register_class ("http://lvtoolkit.org/plugins/workhorse#ui");
