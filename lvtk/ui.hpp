/****************************************************************************

 ui.hpp - Wrapper library to make it easier to write LV2 UIs in C++

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA  02110-1301  USA

 ****************************************************************************/

#ifndef LVTK_LV2_UI_HPP
#define LVTK_LV2_UI_HPP

#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>

#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#include <lvtk/feature.hpp>
#include <lvtk/ext/common.h>
#include <lvtk/ext/data_access.hpp>
#include <lvtk/ext/instance_access.hpp>

#include "private/debug.hpp"
#include "private/ui_features.hpp"

namespace lvtk
{

    /** Cast any pointer into an LV2_Widget */
    #define widget_cast(w) reinterpret_cast<LV2UI_Widget*> (w)

    /** @internal A list of LV2UI_Descriptors. */
    class UIDescList : public std::vector<LV2UI_Descriptor>
    {
    public:
        ~UIDescList();
    };

    /** @internal
     This function returns the list of LV2 descriptors. It should only be
     used internally. */
    UIDescList&
    get_lv2g2g_descriptors();

    /** @defgroup guimixins UI Mixins

        These classes implement extra functionality that you may want to have
        in your UI class, just like the @ref pluginmixins "plugin mixins" do
        for plugin classes. Some of them are template classes with a boolean
        @c Required parameter - if this is true the UI will fail to instantiate
        unless the host supports the extension implemented by that mixin.
     */

    /** @defgroup toolkitmixins Toolkit Mixins
        These classes implement the UI's @c widget() method for a given
        GUI toolkit. For example, the GtkUI mixin auto-creates a base
        container (Gtk::VBox) and returns it via @ widget(). See
        the @ref guimixins "ui mixins" section for other UI extensions
     */

    /** This is the base class for a plugin UI. You should inherit it and
        override any public member functions you want to provide
        implementations for. All subclasses must have a constructor with
        the signature

        where @c plugin_uri is the URI of the plugin that this UI will
        control (not the URI for the UI itself).

        You can extend your UI classes, for example adding support for
        UI extensions, by passing @ref guimixins "UI mixin classes" as template
        parameters to UI (second template parameter and onwards).

        @headerfile lvtk/ui.hpp
     */
    template<class Derived, class Ext1 = end, class Ext2 = end,
             class Ext3 = end, class Ext4 = end, class Ext5 = end,
             class Ext6 = end, class Ext7 = end, class Ext8 = end,
             class Ext9 = end>
    class UI : public MixinTree<Derived, Ext1, Ext2, Ext3, Ext4, Ext5, Ext6,
            Ext7, Ext8, Ext9>
    {
    public:

        /** @internal
            The constructor for the UI class. You should never use it directly.
         */
        inline
        UI()
        {
            m_ctrl = s_ctrl;
            m_wfunc = s_wfunc;
            m_features = s_features;
            m_bundle_path = s_bundle_path;
            s_ctrl = 0;
            s_wfunc = 0;
            s_features = 0;
            s_bundle_path = 0;
            if (m_features)
            {
                feature_handler_map hmap;
                Derived::map_feature_handlers(hmap);
                for (const lvtk::Feature* const * iter = m_features;
                        *iter != 0; ++iter)
                {
                    feature_handler_map::iterator miter;
                    miter = hmap.find((*iter)->URI);
                    if (miter != hmap.end())
                    {
                        miter->second(static_cast<Derived*>(this),
                                (*iter)->data);
                    }
                }
            }
        }

        /** Override this if you want your UI to do something when a control port
            value changes in the plugin instance. */
        inline void
        port_event(uint32_t port, uint32_t buffer_size, uint32_t format,
                void const* buffer)
        {
        }

        /**
            Use this template function to register a class as a LV2 UI.

            @param uri The UIs URI
            @return Descriptor index
         */
        static int
        register_class(char const* uri)
        {
            LV2UI_Descriptor desc;
            std::memset(&desc, 0, sizeof(LV2UI_Descriptor));
            desc.URI = strdup(uri);
            desc.instantiate = &Derived::create_ui_instance;
            desc.cleanup = &Derived::delete_ui_instance;
            desc.port_event = &Derived::_port_event;
            desc.extension_data = &Derived::extension_data;

            get_lv2g2g_descriptors().push_back(desc);
            return get_lv2g2g_descriptors().size() - 1;
        }

    protected:

        /** Send a chunk of data to a plugin port. The format of the chunk is
            determined by the port type and the transfer mechanisms used, you
            should probably use a wrapper function instead such as write_control().
         */
        inline void
        write(uint32_t port, uint32_t buffer_size, uint32_t format,
                void const* buffer)
        {
            (*m_wfunc)(m_ctrl, port, buffer_size, format, buffer);
        }

        /** Set the value of a control input port in the plugin instance. */
        inline void
        write_control(uint32_t port, float value)
        {
            write(port, sizeof(float), 0, &value);
        }

        /** Get the feature array
            @return The host-passed Feature array
            @remarks This may only be valid while the constructor
            is running.
        */
        inline Feature const* const *
        features()
        {
            return m_features;
        }

        /** Get the filesystem path to the bundle that contains this UI. */
        inline char const*
        bundle_path() const
        {
            return m_bundle_path;
        }

    public:
        /** Get the controller
            @return Instance this UI is controlling
            @remarks You only need it if you want to handle extensions
            yourself.
         */
        inline void*
        controller()
        {
            return m_ctrl;
        }

    private:

        // This is quite ugly but needed to allow these mixins to call
        // protected functions in the UI class, which we want.
#if defined (LVTK_EXTRAS_ENABLED)
        friend class WriteMIDI<true>::I<Derived>;
        friend class WriteMIDI<false>::I<Derived>;
        friend class WriteOSC<true>::I<Derived>;
        friend class WriteOSC<false>::I<Derived>;
#endif

        /** @internal
            This function creates an instance of a plugin UI. It is used
            as the instantiate() callback in the LV2 descriptor. You should not use
            it directly. */
        static LV2UI_Handle
        create_ui_instance(LV2UI_Descriptor const* descriptor,
                char const* plugin_uri, char const* bundle_path,
                LV2UI_Write_Function write_func, LV2UI_Controller ctrl,
                LV2UI_Widget* widget, LV2_Feature const* const * features)
        {
            /* Copy some data to static variables so the subclasses don't have to
             bother with it - this is threadsafe since hosts are not allowed
             to instantiate the same plugin concurrently */
            s_ctrl = ctrl;
            s_wfunc = write_func;
            s_features = features;
            s_bundle_path = bundle_path;

            /** Write some debug information */
            if (LVTK_DEBUG)
            {
                std::clog << "[LV2::UI] Creating UI...\n\n"
                        << "  Plugin URI:      \"" << plugin_uri << "\"\n"
                        << "  Bundle path:     \"" << bundle_path << "\"\n"
                        << "  UI Features:\n";
                lvtk::Feature const* const * iter;
                for (iter = features; *iter; ++iter)
                    std::clog << "    \"" << (*iter)->URI << "\"\n";
            }

            // create the UI object
            if (LVTK_DEBUG)
                std::clog << "  Creating LV2 Widget..." << std::endl;
            Derived* ui = new Derived(plugin_uri);
            *widget = ui->widget();

            /** Check all is OK */
            if (ui->check_ok() && *widget != NULL)
            {
                return reinterpret_cast<LV2UI_Handle>(ui);
            }

            delete ui;
            return 0;
        }

        /** @internal
            This function destroys an instance of a UI. It is used as the
            cleanup() callback in the LV2UI descriptor. You should not use it
            directly.
        */
        static void
        delete_ui_instance(LV2UI_Handle instance)
        {
            delete static_cast<Derived*>(instance);
        }

        /** @internal
            This is the main port_event() callback. You should not
            use it directly.
         */
        static void
        _port_event(LV2UI_Handle instance, uint32_t port,
                uint32_t buffer_size, uint32_t format, void const* buffer)
        {
            static_cast<Derived*>(instance)->port_event(port, buffer_size,
                    format, buffer);
        }

        void* m_ctrl;
        LV2UI_Write_Function m_wfunc;
        lvtk::Feature const* const * m_features;
        char const* m_bundle_path;

        static void* s_ctrl;
        static LV2UI_Write_Function s_wfunc;
        static lvtk::Feature const* const * s_features;
        static char const* s_bundle_path;

    };

    /* Yes, static variables are messy. */
    template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
            class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
        void* UI<Derived, Ext1, Ext2, Ext3, Ext4, Ext5, Ext6, Ext7, Ext8, Ext9>::s_ctrl =
                0;

    template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
            class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
        LV2UI_Write_Function UI<Derived, Ext1, Ext2, Ext3, Ext4, Ext5, Ext6,
                Ext7, Ext8, Ext9>::s_wfunc = 0;

    template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
            class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
        lvtk::Feature const* const * UI<Derived, Ext1, Ext2, Ext3, Ext4, Ext5,
                Ext6, Ext7, Ext8, Ext9>::s_features = 0;

    template<class Derived, class Ext1, class Ext2, class Ext3, class Ext4,
            class Ext5, class Ext6, class Ext7, class Ext8, class Ext9>
        char const* UI<Derived, Ext1, Ext2, Ext3, Ext4, Ext5, Ext6, Ext7, Ext8,
                Ext9>::s_bundle_path = 0;

}

#endif /* LVTK_LV2_UI_HPP */
