/* 
    Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#pragma once

#include <lvtk/lvtk.hpp>
#include <lv2/lv2plug.in/ns/ext/options/options.h>

namespace lvtk {
/** Alias to `LV2_Options_Option`
    @headerfile lvtk/option.hpp
    @ingroup options
*/
using Option = LV2_Options_Option;

/** Equivalent to LV2_Options_Context
    @headerfile lvtk/option.hpp
    @ingroup options
*/
enum OptionsContext {
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
};

/** Equivalent to LV2_Options_Status */
enum OptionsStatus {
    OPTIONS_SUCCESS         = LV2_OPTIONS_SUCCESS,          /**< Completed successfully. */
    OPTIONS_ERR_UNKNOWN     = LV2_OPTIONS_ERR_UNKNOWN,      /**< Unknown error. */
    OPTIONS_ERR_BAD_SUBJECT = LV2_OPTIONS_ERR_BAD_SUBJECT,  /**< Invalid/unsupported subject. */
    OPTIONS_ERR_BAD_KEY     = LV2_OPTIONS_ERR_BAD_KEY,      /**< Invalid/unsupported key. */
    OPTIONS_ERR_BAD_VALUE   = LV2_OPTIONS_ERR_BAD_VALUE     /**< Invalid/unsupported value. */
};

/** An array of Options
    @headerfile lvtk/option.hpp
    @ingroup options
*/
class OptionsArray
{
public:
    OptionsArray() {
        opts = (Option*) malloc (sizeof (Option));
        memset (&opts[0], 0, sizeof (Option));
        allocated = true;
        count = 1;
    }

    ~OptionsArray() {
        if (allocated && opts != nullptr) {
            std::free (opts);
            opts = nullptr;
        }
    }

    void add (lvtk::OptionsContext  context, 
              uint32_t              subject, 
              LV2_URID              key,
              uint32_t              size, 
              LV2_URID              type,
              const void*           value)
    {
        add (static_cast<LV2_Options_Context> (context),
             subject, key, size, type, value);
    }

    void add (LV2_Options_Context   context, 
              uint32_t              subject, 
              LV2_URID              key,
              uint32_t              size, 
              LV2_URID              type,
              const void*           value)
    {
        if (! allocated)
            return;
        opts = (Option*) realloc (opts, ++count * sizeof (Option));
        memset (&opts[count - 1], 0, sizeof (Option));
        auto& opt = opts [count - 2];
        opt.context     = context;
        opt.subject     = subject;
        opt.key         = key;
        opt.size        = size;
        opt.type        = type;
        opt.value       = value;
    }

    size_t size() const { return count - 1; }
    const Option* c_obj() const { return opts; }

private:
    bool allocated = false;
    uint32_t count = 0;
    Option* opts = nullptr;
};

/** An simple iterator to use with a plain array of LV2_Options_Option's

    @code
        OptionsIterator iter (get_options());
        while (const Option* opt = iter.next())
        {
            // handle the option
        }
    @endcode
    @headerfile lvtk/option.hpp
    @ingroup options
*/
class OptionsIterator
{
public:
    /** Create a new iterator
        @param options  The options array to scan
     */
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
    mutable uint32_t   index = 0;
    uint32_t           m_size = 0;
    const Option*      p_opts = nullptr;
};

/** The LV2 Options Feature
 
    @headerfile lvtk/option.hpp
    @ingroup options
 */
struct OptionsFeature final {
    OptionsFeature() = default;

    /** Get the options passed by the host as an LV2_Feature

        @note The options array MUST NOT be modified by the plugin instance
        @return The options array or 0 if no options were supplied
     */
    const Option* get_options() const { return p_host_options; }

    /** Assign options from a feature
        @param feature  Should be a feature with LV2_OPTIONS__options
                        as the URI.
        
        @returns True if the feature data was set
     */
    bool set_feature (const Feature& feature) {
        if (strcmp (LV2_OPTIONS__options, feature.URI) == 0) {
            p_host_options = (Option*) feature.data;
            return true;
        }
        return false;
    }

private:
    Option* p_host_options = nullptr;
};

} /* namespace lvtk */
