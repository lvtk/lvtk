/*
    bufsize.hpp - Support file for writing LV2 plugins in C++

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
*/

/** @file bufsize.hpp */

#ifndef LVTK_BUFSIZE_HPP
#define LVTK_BUFSIZE_HPP


#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>

#include <lvtk/ext/options.hpp>
#include <lvtk/ext/urid.hpp>

namespace lvtk {


    struct BufferInfo {
        uint32_t min;
        uint32_t max;
        uint32_t sequence_size;
        bool bounded;
        bool fixed;
        bool power_of_two;
    };

    /** The LV2 BufSize Feature Mixin

        This mixin provides a method that scans options for buffer
        information.

        @note BufSize depends on the URID and Options mixins
        @struct lvtk::Options lvtk/ext/bufsize.hpp
        @ingroup pluginmixins
     */
    template <bool Required = false>
    struct BufSize
    {
        template <class Derived>
        struct I : Extension<Required>
        {
            /** @skip */
            I() : m_checked (false)
            {
                this->m_ok = true;
                memset (&m_info, 0, sizeof (BufferInfo));
            }

            /** @skip */
            static void
            map_feature_handlers (FeatureHandlerMap& hmap)
            {
                hmap[LV2_BUF_SIZE__boundedBlockLength]  = &I<Derived>::handle_bounded;
                hmap[LV2_BUF_SIZE__powerOf2BlockLength] = &I<Derived>::handle_poweroftwo;
                hmap[LV2_BUF_SIZE__fixedBlockLength]    = &I<Derived>::handle_fixed;
            }

            /** @internal Handle the options feature */
            static void
            handle_bounded (void* instance, FeatureData data)
            {
                Derived* plugin (reinterpret_cast<Derived*> (instance));
                I<Derived>* mixin (static_cast<I<Derived>*> (plugin));
                mixin->m_info.bounded = true;
            }

            /** @internal Handle the options feature */
            static void
            handle_poweroftwo (void* instance, FeatureData data)
            {
                Derived* plugin (reinterpret_cast<Derived*> (instance));
                I<Derived>* mixin (static_cast<I<Derived>*> (plugin));
                mixin->m_info.power_of_two = true;
            }

            /** @internal Handle the options feature */
            static void
            handle_fixed (void* instance, FeatureData data)
            {
                Derived* plugin (reinterpret_cast<Derived*> (instance));
                I<Derived>* mixin (static_cast<I<Derived>*> (plugin));
                mixin->m_info.fixed = true;
            }
            /** @internal Sanity check the mixin */
            bool
            check_ok()
            {
                if (LVTK_DEBUG) {
                    std::clog <<"    [BufSize] validation "
                              <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
                }
                return this->m_ok;
            }

        protected:

            /** Extract buffer information from host provided options */
            const BufferInfo&
            get_buffer_info()
            {
                if (! m_checked)
                {
                    Derived* plugin   (static_cast<Derived*> (this));
                    LV2_URID min      (plugin->map (LV2_BUF_SIZE__minBlockLength));
                    LV2_URID max      (plugin->map (LV2_BUF_SIZE__maxBlockLength));
                    LV2_URID seq_size (plugin->map (LV2_BUF_SIZE__sequenceSize));

                    OptionsIter iter (plugin->get_supplied_options());
                    while (const Option* opt = iter.next())
                    {
                        if (min == opt->key)
                            m_info.min = *(uint32_t*) opt->value;
                        if (max == opt->key)
                            m_info.max = *(uint32_t*) opt->value;
                        if (seq_size == opt->key)
                            m_info.sequence_size = *(uint32_t*) opt->value;
                    }

                    m_checked = true;
                }

                return m_info;
            }

        private:

            bool m_checked;
            BufferInfo m_info;

        };
    };

}


#endif /* LVTK_BUFSIZE_HPP */
