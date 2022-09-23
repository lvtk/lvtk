// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/ext/extension.hpp"

#include <lv2/ui/ui.h>

namespace lvtk {

/** Parent feature
    @headerfile lvtk/ext/parent.hpp
    @ingroup ui
*/
struct ParentWidget final : FeatureData<LV2UI_Widget, LV2UI_Widget> {
    ParentWidget() : FeatureData (LV2_UI__parent) {}
};

/** Support for UI Parent
    @headerfile lvtk/ext/parent.hpp
    @ingroup ui
*/
template <class I>
struct Parent : NullExtension {
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

} // namespace lvtk
