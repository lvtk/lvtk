// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#if _WIN32
#ifdef NOMINMAX
#undef NOMINMAX
#endif
#define NOMINMAX
#endif

#ifdef GL_SILENCE_DEPRECATION
#    undef GL_SILENCE_DEPRECATION
#endif
#define GL_SILENCE_DEPRECATION
#define PUGL_DISABLE_DEPRECATED
#include <pugl/gl.h>
#include <pugl/pugl.h>
