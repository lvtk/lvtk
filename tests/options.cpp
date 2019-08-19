
#include "tests.hpp"

using TestFixutre = CPPUNIT_NS::TestFixture;

namespace lvtk {

}

class Opts : public TestFixutre
{
  CPPUNIT_TEST_SUITE (Opts);
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
        lvtk::OptionsArray opts;
        const int num_opts = 4;
        uint32_t values [num_opts];

        for (int i = 0; i < 4; ++i)
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

        CPPUNIT_ASSERT_EQUAL(opts.size(), (size_t) num_opts);
        lvtk::OptionsIterator iter (opts.c_obj());
        while (auto* opt = iter.next()) {
            CPPUNIT_ASSERT_EQUAL (1024, (int) *(uint32_t*) opt->value);
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Opts);
