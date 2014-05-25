/*
    idle_interface.hpp - This file is part of LVTK
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef LVTK_IDLE_INTERFACE_HPP
#define LVTK_IDLE_INTERFACE_HPP

#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

namespace lvtk {

template <bool Required = true>
struct IdleInterface
{
    template <class Derived>
    struct I : Extension<Required>
    {
        /** @skip */
        static void
        map_feature_handlers (FeatureHandlerMap& hmap)
        {
            hmap[LV2_UI__idleInterface] = &I<Derived>::handle_feature;
        }

        /** @skip */
        static void
        handle_feature (void* instance, FeatureData data)
        {
            Derived* d = reinterpret_cast<Derived*> (instance);
            I<Derived>* fe = static_cast<I<Derived>*> (d);
            fe->m_ok = true;
        }

        /** Sanity check the mixin */
        bool
        check_ok()
        {
            if (LVTK_DEBUG) {
                std::clog<<"    [IdleInterface] validation "
                        <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
            }
            return true;
        }

        /** @skip */
        static const void*
        extension_data (const char* uri)
        {
            if (! std::strcmp (uri, LV2_UI__idleInterface)) {
                static LV2UI_Idle_Interface idle_iface = { &I<Derived>::_idle };
                return &idle_iface;
            }

            return 0;
        }

        int idle() { }

    protected:

        /** @internal */
        static int _idle (LV2UI_Handle ui)
        {
            if (Derived* d = reinterpret_cast<Derived*> (ui))
                return d->idle();
            return 1;
        }
    };
};

}

#endif /* LVTK_IDLE_INTERFACE_HPP */
