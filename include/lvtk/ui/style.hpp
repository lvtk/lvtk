// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/weak_ref.hpp>
namespace lvtk {

class Widget;

class Style {
public:
    Style() {
        _weak_status.reset (this);
    }
    ~Style() {
        _weak_status.reset();
    }

    void draw_button (Widget& button) {}

private:
    LVTK_WEAK_REFABLE (Style);
};

} // namespace lvtk
