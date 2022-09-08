// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#ifndef LVTK_BINDINGS_H
#define LVTK_BINDINGS_H

#define LVTK_MT_BYTE_ARRAY      "lvtk.ByteArray"

#if __cplusplus
    #define LVTK_EXTERN extern "C"
#else
    #define LVTK_EXTERN
#endif

#ifdef _WIN32
    #define LVTK_LUALIB LVTK_EXTERN __declspec(dllexport)
#else
    #define LVTK_LUALIB LVTK_EXTERN __attribute__ ((visibility ("default")))
#endif

#endif
