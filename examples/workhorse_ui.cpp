/*
 * worker_ui.cpp
 *
 *  Created on: Sep 30, 2012
 *      Author: mfisher
 */

#include <gtkmm.h>

#include <lvtk/ui.hpp>
#include <lvtk/urid.hpp>

using namespace lvtk;


class workhorse_ui : public UI<workhorse_ui, URID<true> >
{
public:

	workhorse_ui (const char* plugin_uri)
	{
		Gtk::Main::init_gtkmm_internals();
		widget.pack_start (*Gtk::manage(new Gtk::Button()));
		widget.show_all();
	}

	LV2UI_Widget*
	get_widget()
	{
		return widget_cast (widget.gobj());
	}

private:
	Gtk::HBox widget;

};

int _ = workhorse_ui::register_class ("http://lvtoolkit.org/plugins/workhorse#ui");
