// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <array>
#include <cstring>
#include <type_traits>

namespace lvtk {

/** Read a chunk of unaligned memory as value T. */
template <typename T>
static inline T read_unaligned (const void* src) noexcept {
    T value;
    std::memcpy (&value, src, sizeof (T));
    return value;
}

/** Write a value T as unaligned memory. */
template <typename T>
static inline void write_unaligned (void* dst, T value) noexcept {
    std::memcpy (dst, &value, sizeof (T));
}

/** Convert type Val to a std::array<char> */
template <typename Val>
static auto to_chars (Val value) {
    static_assert (std::is_trivial_v<Val>);
    std::array<char, sizeof (Val)> result;
    write_unaligned (result.data(), value);
    return result;
}

} // namespace lvtk
