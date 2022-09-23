
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
    BOOST_REQUIRE_EQUAL (world.plugins().size(), world.plugin_uris().size());

    for (const auto& info : world.plugins()) {
        std::clog << info.name << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()
