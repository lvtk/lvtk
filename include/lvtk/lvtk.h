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

#define LVTK_EXPORT LVTK_EXTERN LVTK_API

#ifndef LVTK_API
#    define LVTK_API
#endif

#ifndef LVTK_EXPORT
#    define LVTK_EXPORT
#endif

#define LVTK_ORG "https://lvtk.org"

#ifdef __linux__
#    define LVTK_UI__NativeUI LV2_UI__X11UI
#elif defined(__APPLE__)
#    define LVTK_UI__NativeUI LV2_UI__CocoaUI
#else
#    define LVTK_UI__NativeUI LV2_UI__WindowsUI
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

namespace lvtk {
template <typename... T>
inline static void ignore (T&&...) noexcept {}
} // namespace lvtk

#endif
