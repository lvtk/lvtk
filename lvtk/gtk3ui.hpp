/*

    gtkui.hpp - Wrapper library to make it easier to write LV2 Gtk3UIs in C++

    Copyright (C) 2006-2008 Lars Luthman <lars.luthman@gmail.com>
    Modified by Dave Robillard, 2008 (URI map mixin)
    Modified by Michael Fisher, 2012 (LV2 rev3)

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

 */

#ifndef LVTK_GTK3UI_HPP
#define LVTK_GTK3UI_HPP

#include <gtkmm.h>
#include <lvtk/ui.hpp>


namespace lvtk {


    /** The Gtk3UI Mixin.
        @headerfile lvtk/gtkui.hpp
        @ingroup guimixins
        @ingroup toolkitmixins
        @see The internal struct I for API details.
     */
    template <bool Required = true>
    struct Gtk3UI
    {
        /** @memberof Gtk3UI */
        template <class Derived>
        struct I : Extension<Required>
        {
            I() : p_container(NULL)
            {
                /* Call before anything else. Prevents glib warnings */
                Gtk::Main::init_gtkmm_internals();

                p_container = Gtk::manage (new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
            }

            ~I()
            {
                /** Container is managed, so don't delete it */
            }

            /** @skip */
            static void
            map_feature_handlers (FeatureHandlerMap& hmap)
            {
                /** Not required or implemented */
            }


            /**
               Sanity check the Mixin
               @return true if the container was created.
             */
            bool
            check_ok()
            {
                return (p_container != NULL);
            }

        protected:

            /**
               Get the underlying Box container Object
               @return Underlying Gtk:HBox
             */
            Gtk::Box& container()
            {
                return *p_container;
            }

            /**
               Add a main widget

               @param widget Gtkmm main UI widget
               @return void
               @remarks You can also use the container() method to acquire the
               underlying Gtk::Box. Do this if you want to add widgets using
               pack_start and pack_end.
             */
            void add (Gtk::Widget& widget)
            {
                p_container->pack_start (widget);
            }

            /**
               Get the Gtk widget as an LV2UI_Widget
               @return The LV2UI_Widget pointer
             */
            LV2UI_Widget*
            widget()
            {
                return widget_cast (p_container->gobj());
            }

        private:
            /** The container object that harbors the main widget */
            Gtk::Box *p_container;

        };
    };


} /* namespace lvtk */


#endif /* LVTK_GTK3UI_HPP */
