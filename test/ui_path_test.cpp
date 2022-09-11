
#include "tests.hpp"
#include <boost/test/unit_test.hpp>
#include <lvtk/ui/path.hpp>

struct PathTest {
    void iterator() {
        using FloatPath = lvtk::graphics::BasicPath<float>;
        FloatPath path;
        int numops = 0;
        path.move (100, 100);
        ++numops;
        path.line (200, 200);
        ++numops;
        path.quad (300, 300, 300, 300);
        ++numops;
        path.cubic (400, 400, 400, 400, 400, 400);
        ++numops;
        path.close();
        ++numops;
        int storedops = 0;
        for (auto& op : path) {
            (void) op;
            ++storedops;
        }
        BOOST_REQUIRE_EQUAL (numops, storedops);
    };
};

BOOST_AUTO_TEST_SUITE (Path)

BOOST_AUTO_TEST_CASE (iterator) {
    PathTest().iterator();
}

BOOST_AUTO_TEST_SUITE_END()
