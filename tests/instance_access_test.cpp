
#include "tests.hpp"

struct InstanceAccessPlug : lvtk::Plugin<InstanceAccessPlug> {
    InstanceAccessPlug (const lvtk::Args& args) : Plugin (args) {}
};

struct InstanceAccessUI : lvtk::UI<InstanceAccessUI, lvtk::InstanceAccess> {
    InstanceAccessUI (const lvtk::UIArgs& args) : UI (args) {}
};

class InstanceAccess : public TestFixutre {
    CPPUNIT_TEST_SUITE (InstanceAccess);
    CPPUNIT_TEST (plugin_instance);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

protected:
    void plugin_instance() {
        lvtk::Descriptor<InstanceAccessPlug> reg ("http://fakeuri.com");
        const auto& desc = lvtk::descriptors().back();
        CPPUNIT_ASSERT (strcmp (desc.URI, "http://fakeuri.com") == 0);
        const LV2_Feature* const features[] = { nullptr };
        auto instance = desc.instantiate (&desc, 44100.0, "/fake/path", features);
        CPPUNIT_ASSERT (instance != nullptr);
        if (! instance)
            return;

        lvtk::UIArgs args;
        args.bundle = "/fake/path";
        args.plugin = "http://fakeuri.com";
        LV2_Feature inst_feature = { LV2_INSTANCE_ACCESS_URI, instance };
        args.features.push_back (inst_feature);

        std::unique_ptr<InstanceAccessUI> ui (new InstanceAccessUI (args));
        CPPUNIT_ASSERT (instance == ui->plugin_instance());

        ui.reset();
        desc.cleanup (instance);
        lvtk::descriptors().pop_back(); // needed so descriptor count test doesn't fail
    }

private:
};

CPPUNIT_TEST_SUITE_REGISTRATION (InstanceAccess);
