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

#define LVTK_ORG "https://lvtk.org"

#define LVTK_PLUGINS_URI       LVTK_ORG "/plugins"
#define LVTK_PLUGINS_PREFIX    LVTK_PLUGINS_URI "/"
#define LVTK_PLUGINS__Volume   LVTK_PLUGINS_PREFIX "volume"
#define LVTK_PLUGINS__VolumeUI LVTK_PLUGINS__Volume "#ui"

#define LVTK_UI_URI     LVTK_ORG "/ns/ui"
#define LVTK_UI_PREFIX  LVTK_UI_URI "#"
#define LVTK_UI__Widget LVTK_UI_PREFIX "Widget"

#ifdef __linux__
#    define LVTK_UI__NativeUI LV2_UI__X11UI
#elif defined(__APPLE__)
#    define LVTK_UI__NativeUI LV2_UI__CocoaUI
#else
#    define LVTK_UI__NativeUI LV2_UI__WindowsUI
#endif

#define LVTK_HOST_URI           LVTK_ORG "/ns/host"
#define LVTK_HOST_PREFIX        LVTK_HOST_URI "#"
#define LVTK_HOST__Main         LVTK_HOST_PREFIX "World"
#define LVTK_HOST__View         LVTK_HOST_PREFIX "Instance"
#define LVTK_HOST__Event        LVTK_HOST_PREFIX "InstanceUI"
#define LVTK_HOST__SupportedUI  LVTK_HOST_PREFIX "SupportedUI"
#define LVTK_HOST__SupportedUIs LVTK_HOST_PREFIX "SupportedUIs"

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
