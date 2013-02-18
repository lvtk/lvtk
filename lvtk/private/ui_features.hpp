/****************************************************************************

    ui_features.hpp - Support file for writing LV2 plugins in C++

    Copyright (C) 2013 Michael Fisher <mfisher31@gmail.com>

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

 ****************************************************************************/


#ifndef LVTK_UI_FEATURES_HPP
#define LVTK_UI_FEATURES_HPP

#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

namespace lvtk {

    /** The Touch Mixin.
        @ingroup guimixins
        @see The internal struct I for details.
     */
    template<bool Required = false>
    struct PortMap
    {
        template<class Derived>
        struct I : Extension<Required>
        {
            I() { memset (&m_pmap, 0, sizeof (LV2UI_Port_Map)); }

            /** @internal */
            static void
            map_feature_handlers (FeatureHandlerMap& hmap)
            {
                hmap[LV2_UI__portIndex] = &I<Derived>::handle_feature;
            }

            /** @internal */
            static void
            handle_feature (void* instance, FeatureData* data)
            {
                Derived* d = reinterpret_cast<Derived*>(instance);
                I<Derived>* mixin = static_cast<I<Derived>*>(d);

                LV2UI_Port_Map* pm (reinterpret_cast<LV2UI_Port_Map*> (data));
                mixin->m_pmap.handle      = pm->handle;
                mixin->m_pmap.port_index  = pm->port_index;
                mixin->m_ok = (pm->handle != NULL && pm->port_index != NULL);
            }

            bool
            check_ok()
            {
                if (! Required)
                    this->m_ok = true;

                if (LVTK_DEBUG)
                {
                    std::clog << "    [PortMap] Validation "
                              << (this->m_ok ? "succeeded" : "failed")
                              << "." << std::endl;
                }

                return this->m_ok;
            }

        protected:

            uint32_t
            port_index (const char* symbol)
            {
                if (portmap_is_valid())
                    return m_pmap.port_index (m_pmap.handle, symbol);
                return LV2UI_INVALID_PORT_INDEX;
            }

        private:
            LV2UI_Port_Map m_pmap;

            /** @internal */
            bool portmap_is_valid()
            {
                return m_pmap.handle != NULL && m_pmap.port_index != NULL;
            }

        };
    };

    /** The Touch Mixin.
        @ingroup guimixins
        @see The internal struct I for details.
     */
    template<bool Required = false>
    struct Touch
    {
        template<class Derived>
        struct I : Extension<Required>
        {
            I() { memset (&m_touch, 0, sizeof (LV2UI_Touch)); }

            /** @internal */
            static void
            map_feature_handlers (FeatureHandlerMap& hmap)
            {
                hmap[LV2_UI__touch] = &I<Derived>::handle_feature;
            }

            /** @internal */
            static void
            handle_feature (void* instance, void* data)
            {
                Derived* d = reinterpret_cast<Derived*>(instance);
                I<Derived>* mixin = static_cast<I<Derived>*>(d);

                LV2UI_Touch* t (reinterpret_cast<LV2UI_Touch*> (data));
                mixin->m_touch.handle = t->handle;
                mixin->m_touch.touch  = t->touch;
                mixin->m_ok = (mixin->m_touch.handle != NULL &&
                               mixin->m_touch.touch  != NULL);
            }

            bool
            check_ok()
            {
                if (! Required)
                    this->m_ok = true;

                if (LVTK_DEBUG)
                {
                    std::clog << "    [Touch] Validation "
                              << (this->m_ok ? "succeeded" : "failed")
                              << "." << std::endl;
                }

                return this->m_ok;
            }

        protected:

            void
            touch (uint32_t port, bool grabbed)
            {
                if (touch_is_valid())
                    m_touch.touch (m_touch.handle, port, grabbed);
            }

        private:
            LV2UI_Touch m_touch;

            /** @internal */
            bool touch_is_valid()
            {
                return m_touch.handle != NULL &&
                       m_touch.touch  != NULL;
            }

        };
    };


    /** The Parent Mixin.
        @ingroup guimixins
        @see The internal struct I for details.
     */
    template<bool Required = false>
    struct Parent
    {
        template<class Derived>
        struct I : Extension<Required>
        {
            I() : p_parent (NULL) { }

            /** @internal */
            static void
            map_feature_handlers(FeatureHandlerMap& hmap)
            {
                hmap[LV2_UI__parent] = &I<Derived>::handle_feature;
            }

            /** @internal */
            static void
            handle_feature(void* instance, void* data)
            {
                Derived* d = reinterpret_cast<Derived*>(instance);
                I<Derived>* mixin = static_cast<I<Derived>*>(d);

                mixin->p_parent = reinterpret_cast<LV2UI_Widget*> (data);
                mixin->m_ok = mixin->p_parent != NULL;
            }

            bool
            check_ok()
            {
                if (! Required)
                    this->m_ok = true;

                if (LVTK_DEBUG)
                {
                    std::clog << "    [Parent] Validation "
                            << (this->m_ok ? "succeeded" : "failed")
                            << "." << std::endl;
                }
                return this->m_ok;
            }

        protected:

            /** Get the parent widget if any

                @return The parent LV2_Widget or NULL if not provided
            */
            LV2UI_Widget*
            get_parent()
            {
               return p_parent;
            }

        private:

            LV2UI_Widget* p_parent;

        };
    };

    /** The UIResize Mixin.
        @ingroup guimixins
        @see The internal struct I for details.
     */
    template<bool Required = true>
    struct UIResize
    {
        template<class Derived>
        struct I : Extension<Required>
        {
            I()
            {
                m_resize.handle    = NULL;
                m_resize.ui_resize = NULL;
            }

            /** @internal */
            static void
            map_feature_handlers(FeatureHandlerMap& hmap)
            {
                hmap[LV2_UI__resize] = &I<Derived>::handle_feature;
            }

            /** @internal */
            static void
            handle_feature(void* instance, void* data)
            {
                Derived* d = reinterpret_cast<Derived*>(instance);
                I<Derived>* mixin = static_cast<I<Derived>*>(d);

                LV2UI_Resize* rz (reinterpret_cast<LV2UI_Resize*> (data));
                mixin->m_resize.handle    = rz->handle;
                mixin->m_resize.ui_resize = rz->ui_resize;
                mixin->m_ok = true;
            }

            bool
            check_ok()
            {
                if (! Required) {
                   this->m_ok = true;
                } else {
                   this->m_ok = rsz_is_valid();
                }

                if (LVTK_DEBUG)
                {
                    std::clog << "    [UIResize] Validation "
                              << (this->m_ok ? "succeeded" : "failed")
                              << "." << std::endl;
                }
                return this->m_ok;
            }

        protected:

            /** */

            bool
            ui_resize(int width, int height)
            {
                if (rsz_is_valid())
                   return (0 == m_resize.ui_resize (m_resize.handle, width, height));

                return false;
            }

        private:
            LV2UI_Resize m_resize;
            bool rsz_is_valid() const { return m_resize.ui_resize != NULL && m_resize.handle != NULL; }

        };
    };

}

#endif /* LVTK_UI_FEATURES_HPP */
