
#include "tests.hpp"

// include examples. order matters!
#include "../examples/volume.cpp"

using TestFixutre = CPPUNIT_NS::TestFixture;

class Registration : public TestFixutre
{
  CPPUNIT_TEST_SUITE (Registration);
  CPPUNIT_TEST (two_descriptors);
  CPPUNIT_TEST (instantiation);
  CPPUNIT_TEST (missing_host_feature);
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
    void two_descriptors() {
        CPPUNIT_ASSERT_EQUAL (lvtk::descriptors().size(), (size_t) 1);
        CPPUNIT_ASSERT_EQUAL (strcmp (lvtk::descriptors()[0].URI, LVTK_VOLUME_URI), (int)0);
        // CPPUNIT_ASSERT_EQUAL (strcmp (lvtk::descriptors()[1].URI, LVTK_WORKHORSE_URI), (int)0);
    }

    void instantiation()
    {
        const auto desc = lvtk::descriptors().front();
        CPPUNIT_ASSERT (strcmp (LVTK_VOLUME_URI, desc.URI) == 0);

        const LV2_Feature* features[] = {
            urids.get_map_feature(),
            urids.get_unmap_feature(),
            nullptr
        };
        
        LV2_Handle handle = desc.instantiate (&desc, 44100.0, "/usr/local/lv2", features);
        CPPUNIT_ASSERT (handle != nullptr);
        if (handle != nullptr) {
            desc.activate (handle);
            
            float control = 0.0f;
            float audio [2][64];

            desc.connect_port (handle, 0, audio[0]);
            desc.connect_port (handle, 1, audio[1]);
            desc.connect_port (handle, 2, audio[0]);
            desc.connect_port (handle, 3, audio[1]);
            desc.connect_port (handle, 4, &control);

            desc.run (handle, 64);
            desc.deactivate (handle);
            desc.cleanup (handle);
        }
    }

    void missing_host_feature()
    {
        const auto& desc = lvtk::descriptors()[0];
        const LV2_Feature* features[] = { nullptr };
        LV2_Handle handle = desc.instantiate (&desc, 44100.0, "/usr/local/lv2", features);
        CPPUNIT_ASSERT (handle == nullptr);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Registration);
