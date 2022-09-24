// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>
#include <vector>

#include <lvtk/host/instance.hpp>
#include <lvtk/lvtk.h>
#include <lvtk/lvtk.hpp>

namespace lvtk {
namespace detail {
/** @private */
class World;
} // namespace detail

/** @defgroup host Host
    Facilities for LV2 hosting.
 */

/** A World of LV2 plugins
    @ingroup host
*/
class LVTK_API World final {
public:
    /** Create a new World. */
    World();
    ~World();

    /** Load all plugins. */
    void load_all();

    std::vector<PluginInfo> plugins() const noexcept;
    std::vector<std::string> plugin_uris() const noexcept;
    std::vector<const LV2_Feature*> features() const noexcept;

    std::unique_ptr<Instance> instantiate (const std::string& uri) const noexcept;

    /** Set the default sample rate to use when instantiating plugins */
    void set_sample_rate (double rate);

private:
    friend class Instance;
    friend class detail::Instance;
    std::unique_ptr<detail::World> impl;
    LVTK_DISABLE_COPY (World);
};

} // namespace lvtk
