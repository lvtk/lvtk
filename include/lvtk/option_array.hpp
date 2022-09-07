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

#include <cassert>
#include <lvtk/ext/options.hpp>

namespace lvtk {

/** An array of Options

    Plugin implementations don't need to use this.  You can, however, use this
    in an LV2 host to easily provide URID map/unmaping features to plugins.

    @headerfile lvtk/option_array.hpp
    @ingroup options
*/
class OptionArray {
public:
    using size_type = uint32_t;
    using pointer = Option*;
    using reference = Option&;
    using const_pointer = const Option*;
    using const_reference = const Option&;

    /** A referenced Option array.

        Creating an array with this contrsuctor will simply reference @p ref
        and NOT allocate memory. This is useful when iterating options provided
        by the host or from get/set in the Options mixin.

        @note You MUST pass a valid pointer according to LV2 specifications

        @param ref  Naked Option array to reference
     */
    OptionArray (const Option* ref)
        : allocated (false), count (0), opts (const_cast<Option*> (ref)) {
        for (;;) {
            auto& opt = ref[count++];
            if (opt.key == 0 || opt.value == nullptr) {
                break;
            }
        }
        assert (count >= 1);
    }

    /** A new Option array.

        Using this ctor will allocate memory and allow `add` to function. It
        will maintain a zeroed end Option so client code doesn't have to
        worry about it.

        This version is intended to be used by an LV2 host.
     */
    OptionArray() {
        opts = (Option*) malloc (sizeof (Option));
        memset (&opts[0], 0, sizeof (Option));
        allocated = true;
        count = 1;
    }

    ~OptionArray() {
        if (allocated && opts != nullptr)
            std::free (opts);
        opts = nullptr;
    }

    /** Add an option. Does nothing if data is referenced */
    OptionArray& add (const Option& option) {
        return add (option.context, option.subject, option.key, option.size, option.type, option.value);
    }

    /** Add an option. Does nothing if data is referenced */
    OptionArray& add (OptionsContext context,
                      uint32_t subject,
                      LV2_URID key,
                      uint32_t size,
                      LV2_URID type,
                      const void* value) {
        if (! allocated)
            return *this;
        opts = (Option*) realloc (opts, ++count * sizeof (Option));
        memset (&opts[count - 1], 0, sizeof (Option));
        auto& opt = opts[count - 2];
        opt.context = context;
        opt.subject = subject;
        opt.key = key;
        opt.size = size;
        opt.type = type;
        opt.value = value;
        return *this;
    }

    /** Returns the number of options stored excluding the zeroed end
        option as per LV2 specifications
     */
    size_type size() const { return count - 1; }

    /** Returns true if empty */
    bool empty() const { return size() == 0; }

    /** Access to Ctype LV2_Options_Option* array */
    const_pointer get() const { return opts; }

    /** @private */
    struct iterator {
        iterator (Option* options, uint32_t index)
            : opts (options), i (index) {}

        const Option& operator*() const { return opts[i]; }
        const Option* operator->() const { return &opts[i]; }

        iterator& operator++() {
            ++i;
            return *this;
        }

        iterator operator++ (int) {
            iterator ret (opts, i);
            ++(*this);
            return ret;
        }

        inline bool operator== (const iterator& other) const { return i == other.i; }
        inline bool operator!= (const iterator& other) const { return i != other.i; }

    private:
        Option* opts = nullptr;
        uint32_t i = 0;
    };

    /** Begin iterator */
    iterator begin() const { return iterator (opts, 0); }

    /** End iterator */
    iterator end() const { return iterator (opts, size()); }

private:
    bool allocated = false;
    uint32_t count = 0;
    Option* opts = nullptr;
};

} // namespace lvtk
