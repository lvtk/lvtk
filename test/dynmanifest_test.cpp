#include <boost/test/unit_test.hpp>

#include "lvtk/dynmanifest.hpp"

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

class DynManifestTest {
public:
    DynManifestTest() {
        manifest.reset ((TestManifest*) lvtk_create_dyn_manifest());
    }

    void subjects() {
        if (auto* file = tmpfile()) {
            std::stringstream ss;
            BOOST_REQUIRE (manifest->get_subjects (ss));
            if (lvtk::write_lines (ss, file)) {
                fseek (file, 0L, SEEK_END);
                BOOST_REQUIRE_EQUAL ((size_t) ss.str().length(), (size_t) ftell (file));
            } else {
                BOOST_REQUIRE_MESSAGE ("couldn't write subject lines", false);
            }
            fclose (file);
        } else {
            BOOST_REQUIRE_MESSAGE ("couldn't open temp FILE", false);
        }
    }

    void get_data() {
        if (auto* file = tmpfile()) {
            std::stringstream ss;
            BOOST_REQUIRE (manifest->get_data (ss, "http://myplugin.org"));
            if (lvtk::write_lines (ss, file)) {
                fseek (file, 0L, SEEK_END);
                BOOST_REQUIRE_EQUAL ((size_t) ss.str().length(), (size_t) ftell (file));
            } else {
                BOOST_REQUIRE_MESSAGE ("couldn't write data lines", false);
            }
            fclose (file);
        } else {
            BOOST_REQUIRE_MESSAGE ("couldn't open temp FILE", false);
        }
    }

private:
    std::unique_ptr<TestManifest> manifest;
};

BOOST_AUTO_TEST_SUITE (DynManifest)

BOOST_AUTO_TEST_CASE (subjects) {
    DynManifestTest().subjects();
}

BOOST_AUTO_TEST_CASE (get_data) {
    DynManifestTest().get_data();
}

BOOST_AUTO_TEST_SUITE_END()
