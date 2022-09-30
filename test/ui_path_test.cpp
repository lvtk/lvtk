
#include "tests.hpp"
#include <boost/test/unit_test.hpp>
#include <lvtk/ui/path.hpp>
#include <lvtk/ui/rectangle.hpp>

struct PathTest {
    void iterator() {
        using FloatPath = lvtk::Path;
        FloatPath path;
        int numops = 0;
        path.move_to (100, 100);
        ++numops;
        path.line_to (200, 200);
        ++numops;
        path.quad_to (300, 300, 300, 300);
        ++numops;
        path.cubic_to (400, 400, 400, 400, 400, 400);
        ++numops;
        path.close_path();
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

BOOST_AUTO_TEST_CASE (rounded_rect) {
    namespace gfx = lvtk::graphics;
    lvtk::Rectangle<float> r;
    float corner_size = 4.f;
    lvtk::Path path;
    gfx::rounded_rect (path, r.x, r.y, r.width, r.height, corner_size, corner_size, true, true, true, true);
}

BOOST_AUTO_TEST_SUITE_END()
