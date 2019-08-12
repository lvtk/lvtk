
#include "tests.hpp"

using TestFixutre = CPPUNIT_NS::TestFixture;

class URID : public TestFixutre
{
  CPPUNIT_TEST_SUITE (URID);
  CPPUNIT_TEST (directory);
  CPPUNIT_TEST (mapping);
  CPPUNIT_TEST (unmapping);
  CPPUNIT_TEST_SUITE_END();

protected:
    lvtk::URIDirectory urids;
    uint32_t urid_A = 0;
    uint32_t urid_B = 0;

public:
    void setUp()
    {
        urid_A = urids.map ("https://dummy.org/A");
        urid_B = urids.map ("https://dummy.org/B");
    }

protected:
    void directory()
    {
        CPPUNIT_ASSERT_EQUAL (urid_A, 1U);
        CPPUNIT_ASSERT_EQUAL (urid_B, 2U);
        CPPUNIT_ASSERT_EQUAL (std::string("https://dummy.org/A"), 
                              std::string(urids.unmap (urid_A)));
        CPPUNIT_ASSERT_EQUAL (std::string("https://dummy.org/B"), 
                              std::string (urids.unmap (urid_B)));

        const auto* map   = (LV2_URID_Map*) urids.get_map_feature()->data;
        const auto* unmap = (LV2_URID_Unmap*) urids.get_unmap_feature()->data;
        CPPUNIT_ASSERT_EQUAL (std::string ("https://dummy.org/A"), 
                              std::string (unmap->unmap (map->handle, urid_A)));
    }

    void mapping()
    {
        lvtk::Map map (*urids.get_map_feature());
        auto* cobj = map.c_obj();
        CPPUNIT_ASSERT (cobj != nullptr);
        CPPUNIT_ASSERT (cobj->handle != nullptr);
        CPPUNIT_ASSERT (cobj->map != nullptr);
        CPPUNIT_ASSERT_EQUAL (map ("https://dummy.org/A"), urid_A);
    }

    void unmapping()
    {
        lvtk::Unmap unmap;
        unmap.set_feature (*urids.get_unmap_feature());
        auto* cobj = unmap.c_obj();
        CPPUNIT_ASSERT (cobj != nullptr);
        CPPUNIT_ASSERT (cobj->handle != nullptr);
        CPPUNIT_ASSERT (cobj->unmap != nullptr);
        CPPUNIT_ASSERT_EQUAL (std::string (unmap (urid_A)), 
                              std::string ("https://dummy.org/A"));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(URID);
