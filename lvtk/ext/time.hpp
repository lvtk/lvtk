/* 
    Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/**
   @file time.hpp
*/
/**
   @page time Time Extension
   C++ convenience header for LV2 time<br />
   LV2 Version Support: >= 1.0 (2012-04-17)<br />

   This is a vocabulary for precisely describing a position in time and
   the passage of time itself, in both real and musical terms.

   In addition to real time (e.g. seconds), two units of time are
   used: "frames" and "beats". A frame is a numbered quantum of time.
   Frame time is related to real-time by the "frame rate" or
   "sample rate", time:framesPerSecond. A beat is a single pulse of
   musical time. Beat time is related to real-time by the "tempo",
   time:beatsPerMinute.

   Musical time additionally has a "meter" which describes passage of
   time in terms of musical "bars". A bar is a higher level grouping of
   beats. The meter describes how many beats are in one bar.

   @link http://lv2plug.in/ns/ext/time/
*/

#pragma once
#include <lv2/time/time.h>
namespace lvtk {} 
