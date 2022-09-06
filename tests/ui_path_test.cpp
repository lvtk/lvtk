
#include "tests.hpp"
#include <lvtk/ui/path.hpp>

class PathTest : public TestFixutre {
    using FloatPath = lvtk::graphics::BasicPath<float>;
    CPPUNIT_TEST_SUITE (PathTest);
    CPPUNIT_TEST (iterator_count);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {}

protected:
    void iterator_count() {
        FloatPath path;
        int numops = 0;
        path.move (100, 100); ++numops;
        path.line (200, 200); ++numops;
        path.quad (300, 300, 300, 300); ++numops;
        path.cubic (400, 400, 400, 400, 400, 400); ++numops;
        path.close(); ++numops;
        int storedops = 0;
        for (auto& op : path) {
            (void)op;
            ++storedops;
        }
        CPPUNIT_ASSERT_EQUAL(numops, storedops);
    };
};

CPPUNIT_TEST_SUITE_REGISTRATION (PathTest);
