// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <memory>
#include <vector>

#include <lvtk/lvtk.h>
#include <lvtk/host/module.hpp>

namespace lvtk {

/** @defgroup host Host
    Facilities for LV2 hosting.
 */

/** @private */
class WorldImpl;

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

private:
    std::unique_ptr<WorldImpl> impl;
    LVTK_DISABLE_COPY(World);};
};
