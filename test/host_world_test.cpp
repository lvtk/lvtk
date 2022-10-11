// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "tests.hpp"
#include <boost/test/unit_test.hpp>
#include <lvtk/host/world.hpp>

BOOST_AUTO_TEST_SUITE (World)

BOOST_AUTO_TEST_CASE (load_all) {
    using World = lvtk::World;
    World world;
    world.load_all();
}

BOOST_AUTO_TEST_CASE (plugins) {
    using World = lvtk::World;
    World world;
    world.load_all();
    auto plugins = world.plugins();
    auto uris    = world.plugin_uris();
    BOOST_REQUIRE_EQUAL (plugins.size(), uris.size());

    BOOST_WARN_MESSAGE (plugins.size() > 0, "No LV2 plugins found on system");
    if (plugins.size() > 0) {
        auto info = plugins.front();
        BOOST_REQUIRE (! info.name.empty());
    }
}

BOOST_AUTO_TEST_CASE (instantiate) {
    using World = lvtk::World;
    World world;
    world.load_all();
    auto plugin = world.instantiate (LVTK_PLUGINS__Volume);
    BOOST_REQUIRE_NE (plugin.get(), nullptr);
    if (plugin) {
        auto ui = plugin->instantiate_ui (0);
        BOOST_REQUIRE_NE (ui.get(), nullptr);
        if (ui) {
            BOOST_REQUIRE_NE (ui->widget(), nullptr);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
