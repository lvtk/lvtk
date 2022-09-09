// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <stdint.h>

typedef union _LvtkPacked {
    struct {
        uint8_t b1, b2, b3, b4;
    } bytes;
    uint8_t data[4];
    lua_Integer packed;
} LvtkPacked;

#ifdef __cplusplus
}
#endif
