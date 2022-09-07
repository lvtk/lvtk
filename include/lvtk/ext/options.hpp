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

/** @defgroup options Options
    Dealing with LV2 Options
*/

#pragma once

#include <lv2/options/options.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {

/** Alias to `LV2_Options_Option`
    @headerfile lvtk/ext/options.hpp
    @ingroup options
*/
using Option = LV2_Options_Option;

/** Equivalent to LV2_Options_Context
    @headerfile lvtk/ext/options.hpp
    @ingroup options
*/
using OptionsContext = LV2_Options_Context;

/** Equivalent to LV2_Options_Status
    @headerfile lvtk/ext/options.hpp
    @ingroup options
*/
using OptionsStatus = LV2_Options_Status;

/** The LV2 Options Feature

    @headerfile lvtk/ext/options.hpp
    @ingroup options
 */
struct OptionsData final : FeatureData<Option> {
    OptionsData() : FeatureData (LV2_OPTIONS__options) {}
};

/** Adds support for LV2 options on your instance
    @headerfile lvtk/ext/options.hpp
    @ingroup options
 */
template <class I>
struct Options : Extension<I> {
    /** @private */
    Options (const FeatureList& features) {
        for (const auto& f : features)
            if (host_options.set (f))
                break;
    }

    /** @returns Options provided by the host or nullptr if not available */
    const Option* options() const { return host_options.get(); }

    /** Get the given options.

        Each element of the passed options array MUST have type, subject, and
        key set. All other fields (size, type, value) MUST be initialised to
        zero, and are set to the option value if such an option is found.

        This function is in the "instantiation" LV2 threading class, so no
        other instance functions may be called concurrently.

        @returns Bitwise OR of OptionsStatus values.
     */
    uint32_t get (const Option* opts) const { return LV2_OPTIONS_SUCCESS; }

    /** Set the given options.

        This function is in the "instantiation" LV2 threading class, so no
        other instance functions may be called concurrently.

        @returns Bitwise OR of OptionsStatus values.
     */
    uint32_t set (const Option* opts) const { return LV2_OPTIONS_SUCCESS; }

protected:
    /** @private */
    inline static void map_extension_data (ExtensionMap& dmap) {
        static const LV2_Options_Interface _options = { _get, _set };
        dmap[LV2_OPTIONS__interface] = &_options;
    }

private:
    OptionsData host_options;

    static uint32_t _get (LV2_Handle handle, LV2_Options_Option* options) {
        return (static_cast<I*> (handle))->get (options);
    }

    static uint32_t _set (LV2_Handle handle, const LV2_Options_Option* options) {
        return (static_cast<I*> (handle))->set (options);
    }
};

} // namespace lvtk
