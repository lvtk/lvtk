
#include "tests.hpp"

#include "lvtk/option_array.hpp"
#include "lvtk/uri_directory.hpp"

#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include <lv2/options/options.h>

#include <cstdint>

class Options : public TestFixutre {
    CPPUNIT_TEST_SUITE (Options);
    CPPUNIT_TEST (array);
    CPPUNIT_TEST_SUITE_END();

protected:
    lvtk::URIDirectory urids;

public:
    void setUp() {
    }

protected:
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

        CPPUNIT_ASSERT_EQUAL (opts.size(), num_opts);

        uint32_t c = 0;
        for (const auto& opt : opts) {
            ++c;
            CPPUNIT_ASSERT_EQUAL (1024, (int) *(uint32_t*) opt.value);
        }

        CPPUNIT_ASSERT_EQUAL (opts.size(), c);

        lvtk::OptionArray opts_ref (opts.get());
        CPPUNIT_ASSERT_EQUAL (opts_ref.size(), opts.size());
        uint32_t fake = 1; // it's referenced, so add should do nothing
        opts_ref.add (LV2_OPTIONS_BLANK, 1, 1, sizeof (uint32_t), 1, &fake);
        CPPUNIT_ASSERT_EQUAL (opts_ref.size(), opts.size());

        for (const auto& opt : opts_ref) // check values are same
            CPPUNIT_ASSERT_EQUAL (1024, (int) *(uint32_t*) opt.value);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (Options);
