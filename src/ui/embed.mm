
#include <lvtk/ui/rectangle.hpp>
#include <lvtk/ui/view.hpp>

#include "ui/detail/embed.hpp"

#include <Cocoa/Cocoa.h>

#include <iostream>

namespace lvtk {
namespace detail {
    Rectangle<float> native_geometry (ViewRef pv) {
        auto view = static_cast<NSView*> ((void*) pv->handle());
        NSRect frame = [view frame];
        Rectangle<CGFloat> r { NSMinX(frame), NSMinY(frame), NSWidth(frame), NSHeight(frame) };
        return r.as<float>();
    }
}}
