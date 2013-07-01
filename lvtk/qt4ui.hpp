/*
    qt4ui.hpp - This file is part of LVTK
    Copyright (C) 2013  Michael Fisher <mfisher@bketech.com>

    Element is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Element is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Element; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/** @file qt4ui.hpp */

#ifndef LVTK_QT4UI_HPP
#define LVTK_QT4UI_HPP

#include <lvtk/ui.hpp>

#include <QWidget>

namespace lvtk {

    /**
       The Qt4UI Mixin.
       @headerfile lvtk/qt4ui.hpp
       @ingroup guimixins
       @ingroup toolkitmixins
       @see The internal struct I for API details.

			 Qt4UI - Implements an LV2 Qt4UI by harboring a user settable QWidget
     */
    template <bool Required = true>
    struct Qt4UI
    {
        /** @memberof Qt4UI */
        template <class Derived>
        struct I : Extension<Required>
        {
            I() : p_widget (0) { }

            ~I()
            {
                if (p_widget && ! p_widget->parent())
                {
#if LVTK_DEBUG
                    std::clog << "[Qt4UI] deleting QWidget\n";
#endif
                    delete p_widget;
                }
            }

            /** @skip */
            static void
            map_feature_handlers (FeatureHandlerMap& hmap)
            {
                /** Not required or implemented */
            }


            /** Sanity check the Mixin
                @return true if the container was created. */
            bool
            check_ok()
            {
                return (p_widget != 0);
            }

        protected:

            /** Add a main widget
                Use this method to set your main widget after its creation.
                Normally, you'll want to call this in your UI's contstructor
                @note The mixin takes ownership and will delete the
                passed in QWidget */
            void 
					  set_widget (QWidget* plugin_gui)
            {
                p_widget = plugin_gui;
            }

            /** Get the QWidget as an LV2UI_Widget
                @return The LV2UI_Widget pointer */
            LV2UI_Widget*
            widget()
            {
                return widget_cast (p_widget);
            }

        private:
            /** @internal The main widget. This is set by client code */
            QWidget *p_widget;

        };
    };


} /* namespace lvtk */


#endif /* LVTK_QT4UI_HPP */
