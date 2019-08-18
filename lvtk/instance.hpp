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

#include <lvtk/lvtk.hpp>
#include <lvtk/atom.hpp>
#include <lvtk/map.hpp>

namespace lvtk {

/** Base class for plugin isntances
    
    Inherrit this and override it's methods to create a minimal LV2 Plugin.
    Add interface mixins to extend functionality.

    @headerfile lvtk/instance.hpp
 */
template<class S, template<class> class... E>
class Instance : public E<S>...
{
protected:
   #if LVTK_STATIC_ARGS
    explicit Instance() : E<S> (args().features)... {}
   #else
    explicit Instance (const Args& args) : E<S> (args.features)... {} 
   #endif

public:
    virtual ~Instance() = default;

    /** Override this to handle activate */
    void activate() {}

    /** Override this to handle connect_port */
    void connect_port (uint32_t, void*) {};

    /** Override this to handle run */
    void run (uint32_t) {}

    /** Override this to handle deactivate */
    void deactivate() {}

    /** Override this to handle cleanup
        Will be called immediately before the instance is deleted
     */
    void cleanup() { }

private:
    friend class Plugin<S>; // so this can be private
    static void map_extension_data (ExtensionMap& em) {
        using pack_context = std::vector<int>;
        pack_context { (E<S>::map_extension_data (em) , 0)... };
    }

   #if LVTK_STATIC_ARGS
    static Args& args() {
        static Args s_args;
        return s_args;
    }
   #endif
};

}

#include <lvtk/ext/extension.hpp>
#include <lvtk/ext/bufsize.hpp>
#include <lvtk/ext/log.hpp>
#include <lvtk/ext/options.hpp>
#include <lvtk/ext/state.hpp>
#include <lvtk/ext/worker.hpp>
