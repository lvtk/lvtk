// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <atomic>

#define PUGL_DISABLE_DEPRECATED
#include <pugl/pugl.h>

#include <lvtk/ui/button.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/slider.hpp>
#include <lvtk/ui/view.hpp>

#include "ui/detail/view.hpp"

namespace lvtk {
namespace detail {

static inline PuglWorldType world_type (Mode mode) {
    if (mode == Mode::PROGRAM)
        return PUGL_PROGRAM;
    else if (mode == Mode::MODULE)
        return PUGL_MODULE;
    return PUGL_MODULE;
}

static inline PuglWorldFlags world_flags() {
    return 0;
}

class Main {
public:
    Main (lvtk::Main& o, const Mode m, std::unique_ptr<lvtk::Backend> b);
    std::unique_ptr<lvtk::View> create_view (lvtk::Widget& widget, ViewFlags flags, uintptr_t parent);

    bool running() const noexcept {
        return first_loop_called
               && last_update_status == PUGL_SUCCESS
               && ! quit_flag;
    }

    bool loop (double timeout);

private:
    friend class lvtk::Main;
    friend class lvtk::detail::Main;
    friend class lvtk::View;
    friend class detail::View;

    lvtk::Main& owner;
    const Mode mode;
    PuglWorld* world { nullptr };
    std::unique_ptr<lvtk::Backend> backend;
    std::vector<lvtk::View*> views;
    std::unique_ptr<lvtk::Style> style;
    bool quit_flag { false };
    std::atomic<int> exit_code { 0 };

    bool first_loop_called { false };
    PuglStatus last_update_status = PUGL_UNKNOWN_ERROR;
};

} // namespace detail
} // namespace lvtk
