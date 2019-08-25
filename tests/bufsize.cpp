
#include "tests.hpp"

struct BufSizePlug : lvtk::Plugin<BufSizePlug, lvtk::BufSize> {
    BufSizePlug (const lvtk::Args& args) : Plugin (args) {}
};

class BufSize : public TestFixutre
{
    CPPUNIT_TEST_SUITE (BufSize);
    CPPUNIT_TEST (buffer_details);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {   
        details.min = 64;
        details.max = 4096;
        details.nominal = 128;
        details.sequence_size = 1024 * 8;

        subject = urid.map ("http://dummy.subject.org");
        type = urid.map ("http://www.w3.org/2001/XMLSchema#nonNegativeInteger");
        options.add (
            LV2_OPTIONS_BLANK, subject,
            urid.map (LV2_BUF_SIZE__minBlockLength),
            sizeof(uint32_t), type,
            &details.min
        ).add (
            LV2_OPTIONS_BLANK, subject,
            urid.map (LV2_BUF_SIZE__maxBlockLength),
            sizeof(uint32_t), type,
            &details.max
        ).add (
            LV2_OPTIONS_BLANK, subject,
            urid.map (LV2_BUF_SIZE__nominalBlockLength),
            sizeof(uint32_t), type,
            &details.nominal
        ).add (
            LV2_OPTIONS_BLANK, subject,
            urid.map (LV2_BUF_SIZE__sequenceSize),
            sizeof(uint32_t), type,
            &details.sequence_size
        );

        args.sample_rate = 44100.0;
        args.bundle = "/fake/bundle.lv2";
        options_feature.URI = LV2_OPTIONS__options;
        options_feature.data = const_cast<lvtk::Option*> (options.c_obj());
        args.features.push_back (options_feature);
        args.features.push_back (*urid.get_map_feature());
        if (details.bounded)
            args.features.push_back (bounded_feature);
    }

protected:
    void buffer_details() {
        auto plugin = std::unique_ptr<BufSizePlug> (new BufSizePlug (args));
        const auto& pdetails = plugin->buffer_details();
        CPPUNIT_ASSERT_EQUAL (details.min,      pdetails.min);
        CPPUNIT_ASSERT_EQUAL (details.max,      pdetails.max);
        CPPUNIT_ASSERT_EQUAL (details.nominal,  pdetails.nominal);
        CPPUNIT_ASSERT_EQUAL (details.fixed,    pdetails.fixed);
        CPPUNIT_ASSERT_EQUAL (details.bounded,  pdetails.bounded);
        CPPUNIT_ASSERT_EQUAL (details.power_of_two,  pdetails.power_of_two);
        CPPUNIT_ASSERT_EQUAL (details.sequence_size,  pdetails.sequence_size);
    }

private:
    lvtk::Args args;
    LV2_Feature bounded_feature = { LV2_BUF_SIZE__boundedBlockLength, nullptr };
    LV2_Feature options_feature = { LV2_OPTIONS__options, nullptr };
    lvtk::BufferDetails details;
    lvtk::OptionArray options;
    lvtk::URIDirectory urid;
    uint32_t subject;
    uint32_t type;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BufSize);
