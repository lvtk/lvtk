// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#ifndef LVTK_UTIL_H
#define LVTK_UTIL_H

#ifdef MIN
    #undef MIN
#endif

#ifdef MAX
    #undef MAX
#endif

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#endif
