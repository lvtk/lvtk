// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "tests.hpp"
#include <boost/test/unit_test.hpp>

#include <lvtk/host/instance.hpp>
#include <lvtk/host/model.hpp>
#include <lvtk/host/node.hpp>
#include <lvtk/host/port.hpp>
#include <lvtk/host/world.hpp>
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/opengl.hpp>

BOOST_AUTO_TEST_SUITE (World)

BOOST_AUTO_TEST_CASE (load_all) {
    lvtk::World world;
    world.load_all();
}

BOOST_AUTO_TEST_CASE (plugins) {
    lvtk::World world;
    world.load_all();
}

BOOST_AUTO_TEST_CASE (nodes) {
    auto world = lilv_world_new();
    lilv_world_load_all (world);

    {
        std::string fakeuri ("http://fakeuri.com");
        auto n1 = lvtk::Node (lilv_new_uri (world, fakeuri.c_str()));
        auto n2 = n1;
        auto n3 = std::move (n2);
        BOOST_REQUIRE_EQUAL (n3.as_uri(), fakeuri);
    }

    lilv_world_free (world);
}

BOOST_AUTO_TEST_SUITE_END()
