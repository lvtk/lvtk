
#include "tests.hpp"

class Options : public TestFixutre
{
  CPPUNIT_TEST_SUITE (Options);
  CPPUNIT_TEST (array);
  CPPUNIT_TEST_SUITE_END();

protected:
    lvtk::URIDirectory urids;

public:
    void setUp()
    {

    }

protected:
    void array() {
        lvtk::OptionArray opts;
        const uint32_t num_opts = 4;
        uint32_t values [num_opts];

        for (uint32_t i = 0; i < num_opts; ++i)
        {
            values [i] = 1024;
            opts.add (LV2_OPTIONS_BLANK,
                urids.map ("http://lvtoolkit.org/ns/lvtk#TestSubject"),
                urids.map ("http://lv2plug.in/ns/ext/buf-size/buf-size.html#maxBlockLength"),
                sizeof (uint32_t),
                urids.map ("http://www.w3.org/2001/XMLSchema#nonNegativeInteger"),
                &values [i]
            );
        }

        CPPUNIT_ASSERT_EQUAL(opts.size(), num_opts);
        
        uint32_t c = 0;
        for (const auto& opt : opts)
        {
            ++c;
            CPPUNIT_ASSERT_EQUAL (1024, (int) *(uint32_t*) opt.value);
        }

        CPPUNIT_ASSERT_EQUAL (opts.size(), c);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Options);
