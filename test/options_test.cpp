
#include "tests.hpp"

#include <boost/test/unit_test.hpp>

#include "lvtk/options.hpp"
#include "lvtk/symbols.hpp"

#include <cstdint>

class OptionsTest {
protected:
    lvtk::URIDirectory urids;

public:
    void array() {
        lvtk::OptionArray opts;
        const uint32_t num_opts = 4;
        uint32_t values[num_opts];

        for (uint32_t i = 0; i < num_opts; ++i) {
            values[i] = 1024;
            opts.add (LV2_OPTIONS_BLANK,
                      urids.map ("http://lvtoolkit.org/ns/lvtk#TestSubject"),
                      urids.map ("http://lv2plug.in/ns/ext/buf-size/buf-size.html#maxBlockLength"),
                      sizeof (uint32_t),
                      urids.map ("http://www.w3.org/2001/XMLSchema#nonNegativeInteger"),
                      &values[i]);
        }

        BOOST_REQUIRE_EQUAL (opts.size(), num_opts);

        uint32_t c = 0;
        for (const auto& opt : opts) {
            ++c;
            BOOST_REQUIRE_EQUAL (1024, (int) *(uint32_t*) opt.value);
        }

        BOOST_REQUIRE_EQUAL (opts.size(), c);

        lvtk::OptionArray opts_ref (opts.get());
        BOOST_REQUIRE_EQUAL (opts_ref.size(), opts.size());
        uint32_t fake = 1; // it's referenced, so add should do nothing
        opts_ref.add (LV2_OPTIONS_BLANK, 1, 1, sizeof (uint32_t), 1, &fake);
        BOOST_REQUIRE_EQUAL (opts_ref.size(), opts.size());

        for (const auto& opt : opts_ref) // check values are same
            BOOST_REQUIRE_EQUAL (1024, (int) *(uint32_t*) opt.value);
    }
};

BOOST_AUTO_TEST_SUITE (Options)

BOOST_AUTO_TEST_CASE (array) {
    OptionsTest().array();
}

BOOST_AUTO_TEST_SUITE_END()
