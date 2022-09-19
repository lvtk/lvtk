// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#ifndef LVTK_H_INCLUDED
#define LVTK_H_INCLUDED

#ifdef __cplusplus
#    define LVTK_EXTERN extern "C"
#else
#    define LVTK_EXTERN
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#    if ! defined(LVTK_STATIC) && defined(LVTK_BUILD)
#        define LVTK_API __declspec(dllexport)
#    elif ! defined(LVTK_STATIC)
#        define LVTK_API __declspec(dllimport)
#    endif
#else
#    if ! defined(LVTK_STATIC)
#        define LVTK_API __attribute__ ((visibility ("default")))
#    endif
#endif

#define LVTK_EXPORT LVTK_EXTERN LVTK_API

#ifndef LVTK_API
#    define LVTK_API
#endif

#ifndef LVTK_EXPORT
#    define LVTK_EXPORT
#endif

#if __cplusplus
} // extern "C"

// clang-format off
#define LVTK_DISABLE_COPY(ClassName)                   \
    ClassName (const ClassName&)            = delete;  \
    ClassName& operator= (const ClassName&) = delete;
#define LVTK_DISABLE_MOVE(ClassName)                   \
    ClassName (const ClassName&&)            = delete; \
    ClassName& operator= (const ClassName&&) = delete;
// clang-format on
#endif

#endif
