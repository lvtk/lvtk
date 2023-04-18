// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "tests.hpp"
#include <boost/test/unit_test.hpp>

#include <lvtk/host/world.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/opengl.hpp>

#include "host/node.hpp"

BOOST_AUTO_TEST_SUITE (World)

BOOST_AUTO_TEST_CASE (load_all) {
    lvtk::World world;
    world.load_all();
}

BOOST_AUTO_TEST_CASE (plugins) {
    lvtk::World world;
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
    lvtk::World world;
    world.load_all();
    auto plugin = world.instantiate (LVTK_PLUGINS__Volume);
    BOOST_REQUIRE_NE (plugin.get(), nullptr);

    if (plugin) {
        BOOST_REQUIRE_NE (plugin->handle(), (lvtk::Handle) nullptr);
        BOOST_REQUIRE_NE (plugin->name(), std::string {});
        BOOST_REQUIRE_NE (plugin->info().name, std::string {});
        BOOST_REQUIRE_NE (plugin->info().URI, std::string {});

        float ports[5][65];
        plugin->activate();
        for (uint32_t i = 0; i < 5; ++i)
            plugin->connect_port (i, ports[i]);
        plugin->run (64);
        plugin->deactivate();
    }
}

BOOST_AUTO_TEST_CASE (instantiate_ui) {
    lvtk::Main ctx (lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>());
    {
        lvtk::Widget widget;
        widget.set_size (1, 1);
        ctx.elevate (widget, 0, 0);
#if LVTK_ELEVATION
        lvtk::ViewRef view = widget.find_view();
        lvtk::World world;
        world.load_all();
        auto plugin = world.instantiate (LVTK_PLUGINS__Volume);
        BOOST_REQUIRE_NE (plugin.get(), nullptr);
        if (plugin && view) {
            auto ui = plugin->instantiate_ui (view->handle());
            BOOST_REQUIRE_NE (ui.get(), nullptr);
            if (ui) {
                BOOST_REQUIRE_NE (ui->widget(), nullptr);
            }
        }
#endif
    }
}

BOOST_AUTO_TEST_CASE (nodes) {
    auto world = lilv_world_new();
    lilv_world_load_all (world);

    {
        std::string fakeuri ("http://fakeuri.com");
        auto n1 = lvtk::lilv::Node (lilv_new_uri (world, fakeuri.c_str()));
        auto n2 = n1;
        auto n3 = std::move (n2);
        BOOST_REQUIRE_EQUAL (n3.as_uri(), fakeuri);
    }

    lilv_world_free (world);
}

BOOST_AUTO_TEST_SUITE_END()
