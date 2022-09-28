
#pragma once

#include <lvtk/ui/widget.hpp>

namespace lvtk {

namespace detail {
class Entry;
}

/** Single line text entry widget */
class Entry : public Widget {
public:
    Entry();
    virtual ~Entry();

private:
    friend class detail::Entry;
    std::unique_ptr<detail::Entry> impl;
};

} // namespace lvtk
