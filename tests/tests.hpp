#pragma once

#include <cppunit/extensions/HelperMacros.h>

#ifndef LVTK_VOLUME_URI
    #define LVTK_VOLUME_URI "http://lvtk.org/plugins/volume"
#endif

#define LVTK_TEST_PLUGIN_URI "http://lvtk.org/plugins/test"
#define LVTK_TEST_UI_URI     "http://lvtk.org/plugins/test#ui"

class TestFixutre : public CPPUNIT_NS::TestFixture {};
