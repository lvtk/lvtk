// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>
#include <string>

#include <lilv/lilv.h>

#include <lvtk/host/model.hpp>

namespace lvtk {
namespace detail {

/** @internal */
struct FreeWorld final {
    void operator() (LilvWorld* world) const {
        lilv_world_free (world);
    }
};

} // namespace detail

/** @defgroup host Host
    C++ wrappers around Lilv and Suil for Plugin and UI hosting.
 */

/** A World of LV2 plugins. This is a slim wrapper around LilvWorld.
 
    @ingroup host
    @headerfile lvtk/host/world.hpp
*/
class World final : private std::unique_ptr<LilvWorld, detail::FreeWorld> {
public:
    /** Create a new World. */
    World() { reset (lilv_world_new()); }

    /** World dtor. */
    ~World() { reset (nullptr); }

    /** Load all plugins. */
    inline void load_all() noexcept { lilv_world_load_all (*this); }

    /** Find a plugin by URI.
     
        @param uri The plugin URI to instantiate
    */
    inline Model find (const std::string& uri) const noexcept {
        auto nuri   = Node (lilv_new_uri (get(), uri.c_str()));
        auto plugin = lilv_plugins_get_by_uri (lilv_world_get_all_plugins (*this), nuri);
        return plugin != nullptr ? Model (plugin) : Model();
    }

    inline operator LilvWorld*() noexcept { return get(); }
    inline operator const LilvWorld*() const noexcept { return get(); }

private:
    LVTK_DISABLE_COPY (World)
};

} // namespace lvtk
