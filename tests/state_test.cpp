
#include "tests.hpp"

struct StatePlug : lvtk::Plugin<StatePlug, lvtk::State> {
    StatePlug (const lvtk::Args& args) : Plugin (args) {}

    static constexpr const uint32_t key = 100;
    static constexpr const uint32_t value = 999;
    static constexpr const uint32_t value_type = 111;

    lvtk::StateStatus save (lvtk::StateStore& store,
                      uint32_t flags, const lvtk::FeatureList& features) {
        auto val = StatePlug::value;
        store (StatePlug::key, &val, sizeof(value), StatePlug::value_type, 0);
        return LV2_STATE_SUCCESS;
    }

    lvtk::StateStatus restore (lvtk::StateRetrieve& retrieve,
                               uint32_t flags,
                               const lvtk::FeatureList& features) {
        retrieve (StatePlug::key);
        return LV2_STATE_SUCCESS;
    }
};

class StateTest : public TestFixutre {
    CPPUNIT_TEST_SUITE (StateTest);
    CPPUNIT_TEST (integration);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

protected:
    void integration() {
        lvtk::Descriptor<StatePlug> reg (LVTK_TEST_PLUGIN_URI);
        const auto& desc = lvtk::descriptors().back();
        CPPUNIT_ASSERT (strcmp (desc.URI, LVTK_TEST_PLUGIN_URI) == 0);

        lvtk::URIDirectory uris;
        uris.get_map_feature();
        uris.get_unmap_feature();

        const LV2_Feature* features[] = { uris.get_map_feature(), uris.get_unmap_feature(), nullptr };
        auto handle = desc.instantiate (&desc, 44100.0, "/fake/path", features);
        CPPUNIT_ASSERT (handle != nullptr);
        auto iface = (const LV2_State_Interface*) desc.extension_data (LV2_STATE__interface);
        CPPUNIT_ASSERT (iface != nullptr);
        
        if (handle && iface) {
            const LV2_Feature* f2[] = { nullptr };
            iface->save (handle, _store, this, 0, f2);
            iface->restore (handle, _retrieve, this, 0, f2);
            CPPUNIT_ASSERT (store_called);
            CPPUNIT_ASSERT (retrieve_called);
        }

        desc.cleanup (handle);
        lvtk::descriptors().pop_back(); // needed so descriptor count test doesn't fail
    }

private:
    bool store_called = false;
    bool retrieve_called = false;
    static LV2_State_Status _store (LV2_State_Handle handle,
                                    uint32_t key,
                                    const void* value,
                                    size_t size,
                                    uint32_t type,
                                    uint32_t flags) {
        auto self = static_cast<StateTest*> (handle);
        self->store_called = true;
        return LV2_STATE_SUCCESS;
    }

    static const void* _retrieve (LV2_State_Handle handle,
                                  uint32_t key,
                                  size_t* size,
                                  uint32_t* type,
                                  uint32_t* flags) {
        auto self = static_cast<StateTest*> (handle);
        self->retrieve_called = true;
        return nullptr;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (StateTest);
