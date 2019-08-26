
#include "tests.hpp"

// dummy plugin with worker interface
struct DataAccessPlug : lvtk::Plugin<DataAccessPlug, lvtk::Worker> {
    DataAccessPlug (const lvtk::Args& args) : Plugin (args) { }
};

// dummy UI that wants data access
struct DataAccessUI : lvtk::UI<DataAccessUI, lvtk::DataAccess> {
    DataAccessUI (const lvtk::UIArgs& args) : UI (args) {}
};

class DataAccess : public TestFixutre
{
    CPPUNIT_TEST_SUITE (DataAccess);
    CPPUNIT_TEST (plugin_extension_data);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {

    }

protected:
    void plugin_extension_data() {
        lvtk::Descriptor<DataAccessPlug> reg ("http://fakeuri.com");
        const auto& desc = lvtk::descriptors().back();
        CPPUNIT_ASSERT(strcmp (desc.URI, "http://fakeuri.com") == 0);
        
        lvtk::UIArgs args;
        args.bundle = "/fake/path";
        args.plugin = "http://fakeuri.com";
        LV2_Extension_Data_Feature data_data;
        data_data.data_access = desc.extension_data;
        LV2_Feature data_feature = { LV2_DATA_ACCESS_URI, &data_data };
        args.features.push_back (data_feature);
        std::unique_ptr<DataAccessUI> ui (new DataAccessUI (args));
        CPPUNIT_ASSERT (nullptr != ui->plugin_extension_data (LV2_WORKER__interface));
        CPPUNIT_ASSERT (nullptr == ui->plugin_extension_data (LV2_STATE__interface));
        ui.reset();
        lvtk::descriptors().pop_back(); // needed so descriptor count test doesn't fail
    }

private:

};

CPPUNIT_TEST_SUITE_REGISTRATION(DataAccess);
