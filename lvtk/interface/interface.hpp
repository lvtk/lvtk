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

#pragma once

#include <functional>
#include <lvtk/lvtk.hpp>

namespace lvtk {

template<class I> struct Interface {};

/** Dummy class indicating an Interface doesn't use the Instance */
struct NoInstance {};

/** This is for extensions which do not provide extension data
    and/or call methods on the Instance 
 */
struct NullInterface : Interface<NoInstance> {
    inline static void map_extension_data (ExtensionMap&) { }
};

}
