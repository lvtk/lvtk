
#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <lv2/core/lv2.h>
#include <lv2/urid/urid.h>

#include <lvtk/ext/urid.hpp>
#include <lvtk/host/uri_directory.hpp>

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
    uint32_t urid_A;
    uint32_t urid_B;

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
        CPPUNIT_ASSERT_EQUAL (map ("https://dummy.org/A"), urid_A);
    }

    void unmapping()
    {
        lvtk::Unmap unmap (*urids.get_unmap_feature());
        CPPUNIT_ASSERT_EQUAL (std::string (unmap (urid_A)), 
                              std::string ("https://dummy.org/A"));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(URID);
