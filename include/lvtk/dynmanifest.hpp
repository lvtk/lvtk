// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdio>
#include <sstream>
#include <string>

#include <lv2/core/lv2.h>
#include <lvtk/detail/dynmanifest.h>

namespace lvtk {
/** Dynamic Manifest helper class

    To create a dynamic manifest, subclass this an implement it's pure virtual
    methods. Then implement lvtk_create_dyn_manifest().
    @ingroup ext
    @headerfile lvtk/dynmanifest.hpp
 */
class DynManifest {
public:
    DynManifest()          = default;
    virtual ~DynManifest() = default;

    /** Get the subjects
        @param lines    Add each line to this stream
        @return false if problems getting subjects
     */
    virtual bool get_subjects (std::stringstream& lines) = 0;

    /** Get the data
        @param uri      The subject URI to get data for
        @param lines    Add each line to this stream
        @return false if problems getting data
     */
    virtual bool get_data (std::stringstream& lines, const std::string& uri) = 0;
};

/** Write a string vector `lines` as lines to `FILE`

    You don't need to use this directly.  The internal dynmanifest
    callbacks use it to return data to the host.

    @ingroup ext
*/
static bool write_lines (const std::stringstream& lines, FILE* fp) {
    const auto data = lines.str();
    return data.size() == fwrite (data.c_str(), sizeof (char), data.size(), fp);
}

} // namespace lvtk

/** Implement this and return your subclassed @ref lvtk::DynManifest object
    @ingroup ext
    @headerfile lvtk/dynmanifest.hpp
*/
void* lvtk_create_dyn_manifest();

extern "C" {

/** @private */
LV2_SYMBOL_EXPORT
int lv2_dyn_manifest_open (LV2_Dyn_Manifest_Handle* handle, const LV2_Feature* const* features) {
    auto* const manifest = lvtk_create_dyn_manifest();
    if (nullptr == manifest)
        return 1;
    *handle = static_cast<LV2_Dyn_Manifest_Handle> (manifest);
    return 0;
}

/** @private */
LV2_SYMBOL_EXPORT
int lv2_dyn_manifest_get_subjects (LV2_Dyn_Manifest_Handle handle, FILE* fp) {
    auto* manifest = static_cast<lvtk::DynManifest*> (handle);
    std::stringstream lines;
    if (! manifest->get_subjects (lines))
        return 1;
    return lvtk::write_lines (lines, fp) ? 0 : 2;
}

/** @private */
LV2_SYMBOL_EXPORT
int lv2_dyn_manifest_get_data (LV2_Dyn_Manifest_Handle handle, FILE* fp, const char* uri) {
    auto* manifest = static_cast<lvtk::DynManifest*> (handle);
    std::stringstream lines;
    if (! manifest->get_data (lines, uri))
        return 1;
    return lvtk::write_lines (lines, fp) ? 0 : 2;
}

/** @private */
LV2_SYMBOL_EXPORT
void lv2_dyn_manifest_close (LV2_Dyn_Manifest_Handle handle) {
    delete static_cast<lvtk::DynManifest*> (handle);
}
}
