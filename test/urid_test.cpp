// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "tests.hpp"

#include <boost/test/unit_test.hpp>

#include "lvtk/ext/urid.hpp"
#include "lvtk/lvtk.hpp"
#include "lvtk/symbols.hpp"

#include <lv2/core/lv2.h>
#include <lv2/log/log.h>
#include <lv2/urid/urid.h>

#include <cstdarg>
#include <cstdint>
#include <string>

namespace lvtk {

class Console {
    Console() {
        log_feature.URI  = LV2_LOG__log;
        log_data.handle  = this;
        log_data.printf  = _printf;
        log_data.vprintf = _vprintf;
    }

    const LV2_Feature* const get_feature() const { return &log_feature; }

private:
    LV2_Feature log_feature;
    LV2_Log_Log log_data;

    inline static int _printf (LV2_Log_Handle handle, uint32_t type, const char*, ...) {
        return 0;
    }

    inline static int _vprintf (LV2_Log_Handle handle, uint32_t type, const char*, va_list va) {
        return 0;
    }
};
} // namespace lvtk

class URIDTest {
    lvtk::Symbols urids;
    uint32_t urid_A = 0;
    uint32_t urid_B = 0;

public:
    URIDTest() {
        urid_A = urids.map ("https://dummy.org/A");
        urid_B = urids.map ("https://dummy.org/B");
    }

    void directory() {
        BOOST_REQUIRE_EQUAL (urid_A, 1U);
        BOOST_REQUIRE_EQUAL (urid_B, 2U);
        BOOST_REQUIRE_EQUAL (std::string ("https://dummy.org/A"),
                             std::string (urids.unmap (urid_A)));
        BOOST_REQUIRE_EQUAL (std::string ("https://dummy.org/B"),
                             std::string (urids.unmap (urid_B)));

        const auto* map   = (LV2_URID_Map*) urids.map_feature()->data;
        const auto* unmap = (LV2_URID_Unmap*) urids.unmap_feature()->data;
        BOOST_REQUIRE_EQUAL (std::string ("https://dummy.org/A"),
                             std::string (unmap->unmap (map->handle, urid_A)));
    }

    void mapping() {
        lvtk::Map map (*urids.map_feature());
        auto* cobj = map.get();
        BOOST_REQUIRE (cobj != nullptr);
        BOOST_REQUIRE (cobj->handle != nullptr);
        BOOST_REQUIRE (cobj->map != nullptr);
        BOOST_REQUIRE_EQUAL (map ("https://dummy.org/A"), urid_A);
    }

    void unmapping() {
        lvtk::Unmap unmap;
        unmap.set (*urids.unmap_feature());
        auto* cobj = unmap.get();
        BOOST_REQUIRE (cobj != nullptr);
        BOOST_REQUIRE (cobj->handle != nullptr);
        BOOST_REQUIRE (cobj->unmap != nullptr);
        BOOST_REQUIRE_EQUAL (std::string (unmap (urid_A)),
                             std::string ("https://dummy.org/A"));
    }
};

BOOST_AUTO_TEST_SUITE (URID)

BOOST_AUTO_TEST_CASE (directory) {
    URIDTest().directory();
}

BOOST_AUTO_TEST_CASE (mapping) {
    URIDTest().mapping();
}

BOOST_AUTO_TEST_CASE (unmapping) {
    URIDTest().unmapping();
}

BOOST_AUTO_TEST_CASE (refer_to) {
    lvtk::Symbols s1;
    lvtk::Symbols s2;
    lvtk::Symbols s3;

    BOOST_REQUIRE (s1.owner());
    s2.refer_to (s1);
    s3.refer_to (s2);
    BOOST_REQUIRE (! s2.owner() && ! s3.owner());

    auto map1 = s1.map ("map1");
    auto map2 = s3.map ("map2");
    BOOST_REQUIRE_EQUAL (s2.unmap (map1), s1.unmap (map1));
    BOOST_REQUIRE_EQUAL (s3.unmap (map1), s1.unmap (map1));
    BOOST_REQUIRE_EQUAL (s3.unmap (map1), s1.unmap (map1));
    BOOST_REQUIRE_EQUAL (s2.unmap (map2), s1.unmap (map2));
    BOOST_REQUIRE_EQUAL (s3.unmap (map2), s1.unmap (map2));
    BOOST_REQUIRE_EQUAL (s3.unmap (map2), s1.unmap (map2));

    s2.refer_to (nullptr, nullptr);
    s3.refer_to (nullptr, nullptr);
    BOOST_REQUIRE_NE (s2.unmap (map1), s1.unmap (map1));
    BOOST_REQUIRE_NE (s3.unmap (map1), s1.unmap (map1));
    BOOST_REQUIRE_NE (s3.unmap (map1), s1.unmap (map1));
    BOOST_REQUIRE_NE (s2.unmap (map2), s1.unmap (map2));
    BOOST_REQUIRE_NE (s3.unmap (map2), s1.unmap (map2));
    BOOST_REQUIRE_NE (s3.unmap (map2), s1.unmap (map2));
}

BOOST_AUTO_TEST_SUITE_END()
