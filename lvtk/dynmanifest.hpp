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

/** @defgroup dynmanifest Dynamic Manifest 
    Dynamic Manifest support

    <h3>Example</h3>
    @code

        #include <lvtk/dynmanifest.hpp>

        class MyManifest : public lvtk::DynManifest {
        public:
            bool get_subjects (std::stringstream& lines) override {
                lines << "@prefix doap:  <http://usefulinc.com/ns/doap#> . " << std::endl
                      << "@prefix lv2:   <http://lv2plug.in/ns/lv2core#> . " << std::endl
                      << "<http://myplugin.org> a lv2:Plugin ;" << std::endl;
                return true;
            }
    
            bool get_data (const std::string& uri, std::vector<std::string>& lines) override {
                lines << "doap:name \"My Plugin\" ;" << std::endl;
                return true;
            }
        };

        DynManifestHandle lvtk_create_dyn_manifest() {
            return new MyManifest();
        }

    @endcode
*/

#pragma once

#include <lv2/lv2plug.in/ns/ext/dynmanifest/dynmanifest.h>
#include <sstream>
#include <cstdio>

namespace lvtk {
/** Dynamic Manifest helper class
    
    To create a dynamic manifest, subclass this an implement it's pure virtual
    methods. Then implement lvtk_create_dyn_manifest().
    @ingroup dynmanifest
    @headerfile lvtk/dynmanifest.hpp
 */
class DynManifest
{
public:
    DynManifest() = default;
    virtual ~DynManifest() = default;

    /** Get the subjects
        @param lines    Add each line to this stream
        @return false if problems getting subjects
     */
    virtual bool get_subjects (std::stringstream& lines) =0;

    /** Get the data
        @param uri      The subject URI to get data for
        @param lines    Add each line to this stream
        @return false if problems getting data
     */
    virtual bool get_data (std::stringstream& lines, const std::string& uri) =0;
};

/** Write a string vector `lines` as lines to `FILE` 
    
    You don't need to use this directly.  The internal dynmanifest
    callbacks use it to return data to the host.

    @ingroup dynmanifest
*/
static void write_lines (const std::stringstream& lines, FILE* fp) {
    const auto data = lines.str();
    fwrite (data.c_str(), sizeof(char), data.size(), fp);
}

}

/** Alias of LV2_Dyn_Manifest_Handle
    @ingroup dynmanifest
    @headerfile lvtk/dynmanifest.hpp
 */
using DynManifestHandle = LV2_Dyn_Manifest_Handle;

/** Implement this and return your subclassed @ref DynManifest object
    @ingroup dynmanifest
    @headerfile lvtk/dynmanifest.hpp
*/
DynManifestHandle lvtk_create_dyn_manifest();

extern "C" {

/** @private */
LV2_SYMBOL_EXPORT
int lv2_dyn_manifest_open (LV2_Dyn_Manifest_Handle *handle, const LV2_Feature *const *features)
{
    auto* const manifest = lvtk_create_dyn_manifest();
    if (nullptr == manifest)
        return 1;
    *handle = static_cast<LV2_Dyn_Manifest_Handle> (manifest);
    return 0;
}

/** @private */
LV2_SYMBOL_EXPORT
int lv2_dyn_manifest_get_subjects (LV2_Dyn_Manifest_Handle handle, FILE *fp)
{
    auto* manifest = static_cast<lvtk::DynManifest*> (handle);
    std::stringstream lines;
    if (! manifest->get_subjects (lines))
        return 1;
    lvtk::write_lines (lines, fp);
    return 0;
}

/** @private */
LV2_SYMBOL_EXPORT
int lv2_dyn_manifest_get_data (LV2_Dyn_Manifest_Handle handle, FILE *fp, const char *uri)
{
    auto* manifest = static_cast<lvtk::DynManifest*> (handle);
    std::stringstream lines;
    if (! manifest->get_data (lines, uri))
        return 1;
    lvtk::write_lines (lines, fp);
    return 0;
}

/** @private */
LV2_SYMBOL_EXPORT
void lv2_dyn_manifest_close (LV2_Dyn_Manifest_Handle handle)
{
    delete static_cast<lvtk::DynManifest*> (handle);
}

}
