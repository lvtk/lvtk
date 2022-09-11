
#include "tests.hpp"

#include "lvtk/dynmanifest.hpp"

#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstdio>
#include <memory>
#include <ostream>
#include <string>

class TestManifest : public lvtk::DynManifest {
public:
    bool get_subjects (std::stringstream& lines) override {
        lines << "@prefix doap:  <http://usefulinc.com/ns/doap#> . " << std::endl
              << "@prefix lv2:   <http://lv2plug.in/ns/lv2core#> . " << std::endl
              << "<http://myplugin.org> a lv2:Plugin ;" << std::endl;
        return true;
    }

    bool get_data (std::stringstream& lines, const std::string& uri) override {
        lines << "doap:name \"My Plugin\" ;" << std::endl;
        return true;
    }
};

void* lvtk_create_dyn_manifest() {
    return new TestManifest();
}

class DynManifest : public TestFixutre {
    CPPUNIT_TEST_SUITE (DynManifest);
    CPPUNIT_TEST (subjects);
    CPPUNIT_TEST (get_data);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        manifest.reset ((TestManifest*) lvtk_create_dyn_manifest());
    }

protected:
    void subjects() {
        if (auto* file = tmpfile()) {
            std::stringstream ss;
            CPPUNIT_ASSERT (manifest->get_subjects (ss));
            if (lvtk::write_lines (ss, file)) {
                fseek (file, 0L, SEEK_END);
                CPPUNIT_ASSERT_EQUAL ((size_t) ss.str().length(), (size_t) ftell (file));
            } else {
                CPPUNIT_ASSERT_MESSAGE ("couldn't write subject lines", false);
            }
            fclose (file);
        } else {
            CPPUNIT_ASSERT_MESSAGE ("couldn't open temp FILE", false);
        }
    }

    void get_data() {
        if (auto* file = tmpfile()) {
            std::stringstream ss;
            CPPUNIT_ASSERT (manifest->get_data (ss, "http://myplugin.org"));
            if (lvtk::write_lines (ss, file)) {
                fseek (file, 0L, SEEK_END);
                CPPUNIT_ASSERT_EQUAL ((size_t) ss.str().length(), (size_t) ftell (file));
            } else {
                CPPUNIT_ASSERT_MESSAGE ("couldn't write data lines", false);
            }
            fclose (file);
        } else {
            CPPUNIT_ASSERT_MESSAGE ("couldn't open temp FILE", false);
        }
    }

private:
    std::unique_ptr<TestManifest> manifest;
};

CPPUNIT_TEST_SUITE_REGISTRATION (DynManifest);
