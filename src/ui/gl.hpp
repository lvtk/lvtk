// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#ifdef GL_SILENCE_DEPRECATION
#    undef GL_SILENCE_DEPRECATION
#endif
#define GL_SILENCE_DEPRECATION

#if _WIN32
#    include "glad/glad.h"
#    define PUGL_NO_INCLUDE_GL_H 1
#    ifdef NOMINMAX
#        undef NOMINMAX
#    endif
#    define NOMINMAX
#endif

#define PUGL_DISABLE_DEPRECATED
#include <pugl/gl.h>
#include <pugl/pugl.h>
