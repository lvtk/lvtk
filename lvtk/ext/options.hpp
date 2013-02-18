/****************************************************************************

    options.hpp - Support file for writing LV2 plugins in C++

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

/** @file options.hpp
    LV2 Compatibility: LV2 Options v1.0 (2012-10-14)
 */

#ifndef LVTK_OPTIONS_HPP
#define LVTK_OPTIONS_HPP

#include <lv2/lv2plug.in/ns/ext/options/options.h>


#ifndef LVTK_OPTIONS_IFACE
#define LVTK_OPTIONS_IFACE 1
#endif

namespace lvtk {


    /** Convenience enum to get LV2_Options_Option into a C++ namespace */
    typedef LV2_Options_Option Option;


    /** Convenience enum to get LV2_Options_Context into a C++ namespace */
    typedef enum {
        /** This option applies to the instance itself.  The subject must be
            ignored. */
        OPTIONS_INSTANCE    = LV2_OPTIONS_INSTANCE,

        /** This option applies to some named resource.  The subject is a URI mapped
            to an integer (a LV2_URID, like the key) */
        OPTIONS_RESOURCE    = LV2_OPTIONS_RESOURCE,

        /** This option applies to some blank node.  The subject is a blank node
            identifier, which is valid only within the current local scope. */
        OPTIONS_BLANK       = LV2_OPTIONS_BLANK,

        /** This option applies to a port on the instance.  The subject is the
            port's index. */
        OPTIONS_PORT        = LV2_OPTIONS_PORT
    } OptionsContext;


    /** Convenience enum to get LV2_Worker_Status into a C++ namespace */
    typedef enum {
        OPTIONS_SUCCESS         = LV2_OPTIONS_SUCCESS,          /**< Completed successfully. */
        OPTIONS_ERR_UNKNOWN     = LV2_OPTIONS_ERR_UNKNOWN,      /**< Unknown error. */
        OPTIONS_ERR_BAD_SUBJECT = LV2_OPTIONS_ERR_BAD_SUBJECT,  /**< Invalid/unsupported subject. */
        OPTIONS_ERR_BAD_KEY     = LV2_OPTIONS_ERR_BAD_KEY,      /**< Invalid/unsupported key. */
        OPTIONS_ERR_BAD_VALUE   = LV2_OPTIONS_ERR_BAD_VALUE     /**< Invalid/unsupported value. */
    } OptionsStatus;


    class OptionsIter
    {
    public:

        OptionsIter (const Option* options)
            : index (0),m_size (0), p_opts (options)
        {
            while (NULL != next())
                ++m_size;
            index = 0;
        }

        /** */
        const Option* next()
        {
            if (p_opts == NULL || (p_opts[index].key   == 0 &&
                                   p_opts[index].value == 0))
                return NULL;

            return &p_opts[index++];
        }

        uint32_t size()     const       { return m_size; }

    private:

        /** @internal The current option index */
        uint32_t           index;

        /** @internal The number of options in the array */
        uint32_t           m_size;

        /** @internal Non constant iteration pointer */
        const Option*      p_opts;

    };


    /** The LV2 Options Feature Mixin

       This mixin provides a callback handler for the LV2 Options feature and
       hooks up the LV2_Options_Interface as extension data.

       @struct lvtk::Options lvtk/ext/options.hpp
       @ingroup pluginmixins
       @ingroup guimixins
     */
    template <bool Required = false>
    struct Options
    {

        template <class Derived>
        struct I : Extension<Required>
        {

            /** @skip */
            I() : p_supplied_opts (NULL) { }

            /** @skip */
            static void
            map_feature_handlers (FeatureHandlerMap& hmap)
            {
                hmap[LV2_OPTIONS__options] = &I<Derived>::handle_feature;
            }

            /** @internal Handle the options feature */
            static void
            handle_feature (void* instance, FeatureData data)
            {
                Derived* plugin (reinterpret_cast<Derived*> (instance));
                I<Derived>* mixin (static_cast<I<Derived>*> (plugin));
                mixin->p_supplied_opts = (Option*) data;
                mixin->m_ok = true;
            }

            /** @internal Sanity check the mixin */
            bool
            check_ok()
            {
                if (LVTK_DEBUG) {
                    std::clog <<"    [Options] validation "
                              <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
                }
                return this->m_ok;
            }

            /** @skip */
            static const void*
            extension_data (const char* uri)
            {
               #if LVTK_OPTIONS_IFACE
                if (! strcmp (uri, LV2_OPTIONS__interface)) {
                    static LV2_Options_Interface options = { &I<Derived>::_get,
                                                             &I<Derived>::_set };
                    return &options;
                }
               #endif
                return 0;
            }

            /** Get the options passed by the host as an LV2_Feature

                @note The options array MUST NOT be modified by the plugin instance
                @return The options array or NULL if no options were supplied
             */
            const Option* get_supplied_options()       { return p_supplied_opts; }

        protected:

            /** Get the given options.

                Each element of the passed options array MUST have type, subject, and
                key set.  All other fields (size, type, value) MUST be initialised to
                zero, and are set to the option value if such an option is found.

                This method corresponds with LV2_Options_Interface::get

                @return Bitwise OR of OptionsStatus values.
            */
            uint32_t get_options (Option*)              { return OPTIONS_SUCCESS; }




            /** Set the given options.

                This function is in the "instantiation" LV2 threading class, so no other
                instance functions may be called concurrently.

                This method corresponds with LV2_Options_Interface::set

                @return Bitwise OR of OptionsStatus values.
            */
            uint32_t set_options (const Option*)        { return OPTIONS_SUCCESS; }

        private:

            /** @internal Options supplied by the host.
                This is non-const because it will be set after the ctor is
                called
            */
            Option* p_supplied_opts;

            /* LV2 Options Implementation */

            static uint32_t _get (LV2_Handle handle, LV2_Options_Option* options)
            {
                Derived* plugin (reinterpret_cast<Derived*> (handle));
                return plugin->get_options (options);
            }

            static uint32_t _set (LV2_Handle handle, const LV2_Options_Option* options)
            {
                Derived* plugin (reinterpret_cast<Derived*> (handle));
                return plugin->set_options (options);
            }

        };
    };

} /* namespace lvtk */

#endif /* LVTK_OPTIONS_HPP */
