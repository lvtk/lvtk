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

#include <lv2/ui/ui.h>
#include <lvtk/ext/extension.hpp>

namespace lvtk {

/** Parent feature
    @headerfile lvtk/ext/ui/parent.hpp
    @ingroup ui
*/
struct ParentWidget final : FeatureData<LV2UI_Widget, LV2UI_Widget> {
    ParentWidget() : FeatureData (LV2_UI__parent) {}
};

/** Support for UI Parent
    @headerfile lvtk/ext/ui/parent.hpp
    @ingroup ui
*/
template<class I> 
struct Parent : NullExtension
{
    /** @private */
    Parent (const FeatureList& features) {
        for (const auto& f : features)
            if (parent.set (f))
                break;
    }

protected:
    /** Returns the parent widget, or nullptr if not found. 
        It is a function object and also has a bool() operator, so....
        @code
        // ...
        
        if (parent) {
            auto* widget = reinterpret_cast<WidgetType*> (parent())
            // do something with the parent widget, WidgetType* above
            // would be an object of whatever toolkit you're using.
        }

        // ...
        @endcode
     */
    ParentWidget parent;
};

}
