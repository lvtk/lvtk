// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/symbols.hpp>

namespace lvtk {

class Context {
protected:
    Context() = default;

public:
    ~Context() = default;
    Symbols& symbols() { return _symbols; }

private:
    Symbols _symbols;
};

} // namespace lvtk
