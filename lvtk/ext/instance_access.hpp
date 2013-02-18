/**
 instance_access.hpp - support file for writing LV2 plugins in C++

 Copyright (C) 2012 Michael Fisher <mfisher31@gmail.com>

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
 */
/**
 @file instance_access.hpp
 C++ convenience header for the LV2 instance access extension.
 LV2 Support: 1.6 (2012-04-17)
 */

#ifndef LVTK_LV2_INSTANCE_ACCESS_HPP
#define LVTK_LV2_INSTANCE_ACCESS_HPP

#include <lv2/lv2plug.in/ns/ext/instance-access/instance-access.h>

#include <lvtk/private/types.hpp>

namespace lvtk
{

    /** The Instance Access Mixin
        @headerfile lvtk/ext/instance_access.hpp
        @ingroup guimixins
        @see The internal struct I for details.
     */
    template<bool Required = true>
    struct InstanceAccess
    {
        template<class Derived>
        struct I : Extension<Required>
        {

            I() :
                    p_plugin_instance(NULL)
            {
            }

            /* ================= Mixin API ========================= */

            /** @internal */
            static void
            map_feature_handlers(FeatureHandlerMap& hmap)
            {
                hmap[LV2_INSTANCE_ACCESS_URI] =
                        &I<Derived>::handle_feature;
            }

            /** @internal */
            static void
            handle_feature(LV2UI_Handle instance, FeatureData data)
            {
                Derived* derived = reinterpret_cast<Derived*>(instance);
                I<Derived>* mixin = static_cast<I<Derived>*>(derived);

                mixin->p_plugin_instance =
                        reinterpret_cast<LV2_Handle>(data);

                mixin->m_ok = (mixin->p_plugin_instance != NULL);
            }

            bool
            check_ok()
            {
                if (!Required)
                    this->m_ok = true;

                if (LVTK_DEBUG)
                {
                    std::clog << "    [InstanceAccess] Validation "
                              << (this->m_ok ? "succeeded" : "failed")
                              << "." << std::endl;
                }

                return this->m_ok;
            }

        protected:

            /** Get the plugin instance */
            LV2_Handle
            get_instance()
            {
                return p_plugin_instance;
            }

            /** @internal Feature Data passed from host */
            LV2_Handle p_plugin_instance;
        };
    };
} /* namespace lvtk */

#endif /* LVTK_LV2_INSTANCE_ACCESS_HPP */
