// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>
#include <string>

namespace lvtk {

class Font final {
public:
    enum Style {
        NORMAL      = 0,
        BOLD        = (1 << 0),
        ITALIC      = (1 << 1),
        UNDERLINE   = (1 << 2)
    };

    Font()  = default;
    ~Font() = default;

    float height() const noexcept { return _state.height; }
    bool normal() const noexcept { return _state.flags == NORMAL; }
    bool bold() const noexcept { return (_state.flags & BOLD) != 0; }
    bool italic() const noexcept { return (_state.flags & ITALIC) != 0; }
    bool underline() const noexcept { return (_state.flags & UNDERLINE) != 0; }

private:
    struct State {
        float height { 15.f };
        uint32_t flags { NORMAL };
        std::string family { "<sans-serif>" };
        State (const State& o) { operator=(o); }
        State& operator= (const State& o) {
            height = o.height;
            flags = o.flags;
            return *this;
        }
    };
     _state;
};

} // namespace lvtk
