/*
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
*/

/** @file options.hpp
    LV2 Compatibility: LV2 Options v1.0 (2012-10-14)
 */

#pragma once

#include <lvtk/lvtk.hpp>
#include <lv2/options/options.h>

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


/** Convenience enum to get LV2_Options_Status into a C++ namespace */
typedef enum {
    OPTIONS_SUCCESS         = LV2_OPTIONS_SUCCESS,          /**< Completed successfully. */
    OPTIONS_ERR_UNKNOWN     = LV2_OPTIONS_ERR_UNKNOWN,      /**< Unknown error. */
    OPTIONS_ERR_BAD_SUBJECT = LV2_OPTIONS_ERR_BAD_SUBJECT,  /**< Invalid/unsupported subject. */
    OPTIONS_ERR_BAD_KEY     = LV2_OPTIONS_ERR_BAD_KEY,      /**< Invalid/unsupported key. */
    OPTIONS_ERR_BAD_VALUE   = LV2_OPTIONS_ERR_BAD_VALUE     /**< Invalid/unsupported value. */
} OptionsStatus;

/** An simple iterator to use with and array of LV2_Options_Option's

    @code
        OptionsIterator iter (get_options());
        while (const Option* opt = iter.next())
        {
            // handle the option
        }
    @endcode
*/

class OptionsIterator
{
public:
    OptionsIterator (const Option* options)
        : index (0),m_size (0), p_opts (options)
    {
        while (0 != next())
            ++m_size;
        index = 0;
    }

    /** Returns the next option or NULL if the end of the list is reached */
    const Option* next() const
    {
        if (p_opts == 0 || (p_opts[index].key   == 0 &&
                                p_opts[index].value == 0))
            return 0;

        return &p_opts [index++];
    }

    /** Returns the total number of options */
    uint32_t size() const { return m_size; }

private:
    /** @internal The current option index */
    mutable uint32_t   index;

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
struct OptionsArray
{
    /** @skip */
    OptionsArray() = default;

    /** Get the options passed by the host as an LV2_Feature

        @note The options array MUST NOT be modified by the plugin instance
        @return The options array or 0 if no options were supplied
     */
    const Option* get_options() const { return p_host_options; }
    const Option* c_obj()       const { return get_options(); }
    
    void set_feature (const Feature& feature) {
        if (strcmp (LV2_OPTIONS__options, feature.URI) == 0)
            p_host_options = (Option*) feature.data;
    }

private:
    Option* p_host_options = nullptr;
};

#if 0
template<class InstanceType>
struct OptionsInterface 
{
    OptionsInterface()
    {
        static LV2_Options_Interface options = { _get, _set };
        Plugin<InstanceType>::register_extension (
            LV2_OPTIONS__interface, &options
        );
    }

private:
    static uint32_t _get (LV2_Handle handle, LV2_Options_Option* options)
    {
        auto* instance (reinterpret_cast<InstanceType*> (handle));
        return instance->get_options (options);
    }

    static uint32_t _set (LV2_Handle handle, const LV2_Options_Option* options)
    {
        auto* instance (reinterpret_cast<InstanceType*> (handle));
        return instance->set_options (options);
    }
};
#endif

} /* namespace lvtk */
