
#include <lvtk/string.hpp>

#define BOOST_TEST_MODULE Unit
#include <boost/test/included/unit_test.hpp>

#define LOREM_IPSUM \
    R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ut dolor sed lectus condimentum scelerisque ut at ex. Aenean feugiat velit sodales tempus condimentum. Nam sed neque velit. Nulla pretium ut nulla a placerat. Aliquam erat volutpat. Fusce volutpat, urna ut aliquet finibus, nunc mauris porta lacus, ut lacinia dolor sapien ut enim. Vestibulum quis diam mattis, laoreet augue ut, tincidunt magna. Duis semper sit amet leo gravida semper. Lorem ipsum dolor sit amet, consectetur adipiscing elit.)"

using namespace lvtk;

BOOST_AUTO_TEST_SUITE (Strings)

BOOST_AUTO_TEST_CASE (ctors) {
    auto a = String();
    a = String ("lv2");
    a = String (String ("rules"));
    BOOST_REQUIRE_EQUAL (a.str(), "rules");
}

BOOST_AUTO_TEST_CASE (streams) {
    String s1 = "hello";
    BOOST_REQUIRE_EQUAL (s1, "hello");
    s1 << " " << String ("world");
    BOOST_REQUIRE_EQUAL (s1, "hello world");

    s1.clear();
    s1 << int (1);
    BOOST_REQUIRE_EQUAL (s1.str(), "1");

    s1.clear();
    s1 << char ('B');
    BOOST_REQUIRE_EQUAL (s1.str(), "B");

    s1.clear();
    s1 << int64_t (100);
    BOOST_REQUIRE_EQUAL (s1.str(), "100");

    s1.clear();
    s1 << double (1.555000);
    BOOST_REQUIRE_EQUAL (s1.str(), "1.555000");

    s1.clear();
    s1 << float (1.444000);
    BOOST_REQUIRE_EQUAL (s1.str(), "1.444000");

    s1.clear(); s1 << LOREM_IPSUM;
    BOOST_REQUIRE_EQUAL (s1, LOREM_IPSUM);
}

BOOST_AUTO_TEST_SUITE_END()
