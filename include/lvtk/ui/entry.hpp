// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/ui/widget.hpp>

namespace lvtk {

namespace detail {
class Entry;
}

/** Single line text entry widget */
class LVTK_API Entry : public Widget {
public:
    Entry();
    virtual ~Entry();

private:
    bool obstructed (int x, int y) override {
        return true;
    }

    /** @private */
    bool text_entry (const TextEvent& ev) override;
    /** @private */
    void paint (Graphics& g) override;
    /** @private */
    void pressed (const Event& ev) override;

    friend class detail::Entry;
    std::unique_ptr<detail::Entry> impl;
};

} // namespace lvtk
