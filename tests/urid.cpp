
#include "tests.hpp"

namespace lvtk {

class Console
{
    Console()
    {
        log_feature.URI     = LV2_LOG__log;
        log_data.handle     = this;
        log_data.printf     = _printf;
        log_data.vprintf    = _vprintf;
    }

    const LV2_Feature* const get_feature() const { return &log_feature; }

private:
    LV2_Feature log_feature;
    LV2_Log_Log log_data;

    inline static int _printf (LV2_Log_Handle handle, uint32_t type, const char*, ...) {
        return 0;
    }

    inline static int _vprintf (LV2_Log_Handle handle, uint32_t type, const char*, va_list va) {
        return 0;
    }
};
}

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
        auto* cobj = map.get();
        CPPUNIT_ASSERT (cobj != nullptr);
        CPPUNIT_ASSERT (cobj->handle != nullptr);
        CPPUNIT_ASSERT (cobj->map != nullptr);
        CPPUNIT_ASSERT_EQUAL (map ("https://dummy.org/A"), urid_A);
    }

    void unmapping()
    {
        lvtk::Unmap unmap;
        unmap.set (*urids.get_unmap_feature());
        auto* cobj = unmap.get();
        CPPUNIT_ASSERT (cobj != nullptr);
        CPPUNIT_ASSERT (cobj->handle != nullptr);
        CPPUNIT_ASSERT (cobj->unmap != nullptr);
        CPPUNIT_ASSERT_EQUAL (std::string (unmap (urid_A)), 
                              std::string ("https://dummy.org/A"));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(URID);
