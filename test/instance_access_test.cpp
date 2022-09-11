
#include <boost/test/unit_test.hpp>

#include "lvtk/ext/instance_access.hpp"
#include "lvtk/lvtk.hpp"
#include "lvtk/plugin.hpp"
#include "lvtk/ui.hpp"

#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include <lv2/core/lv2.h>
#include <lv2/instance-access/instance-access.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

struct InstanceAccessPlug : lvtk::Plugin<InstanceAccessPlug> {
    InstanceAccessPlug (const lvtk::Args& args) : Plugin (args) {}
};

struct InstanceAccessUI : lvtk::UI<InstanceAccessUI, lvtk::InstanceAccess> {
    InstanceAccessUI (const lvtk::UIArgs& args) : UI (args) {}
};

class InstanceAccessTest {
public:
    void plugin_instance() {
        lvtk::Descriptor<InstanceAccessPlug> reg ("http://fakeuri.com");
        const auto& desc = lvtk::descriptors().back();
        BOOST_REQUIRE (strcmp (desc.URI, "http://fakeuri.com") == 0);
        const LV2_Feature* const features[] = { nullptr };
        auto instance = desc.instantiate (&desc, 44100.0, "/fake/path", features);
        BOOST_REQUIRE (instance != nullptr);
        if (! instance)
            return;

        lvtk::UIArgs args;
        args.bundle = "/fake/path";
        args.plugin = "http://fakeuri.com";
        LV2_Feature inst_feature = { LV2_INSTANCE_ACCESS_URI, instance };
        args.features.push_back (inst_feature);

        std::unique_ptr<InstanceAccessUI> ui (new InstanceAccessUI (args));
        BOOST_REQUIRE (instance == ui->plugin_instance());

        ui.reset();
        desc.cleanup (instance);
        lvtk::descriptors().pop_back(); // needed so descriptor count test doesn't fail
    }

private:
};

BOOST_AUTO_TEST_SUITE (InstanceAccess)

BOOST_AUTO_TEST_CASE (plugin_instance) {
    InstanceAccessTest().plugin_instance();
}

BOOST_AUTO_TEST_SUITE_END()
