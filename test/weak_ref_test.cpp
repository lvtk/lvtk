
#include "lvtk/weak_ref.hpp"
#include "tests.hpp"
#include <boost/test/unit_test.hpp>
#include <memory>

struct WeakRefTest {
    void basics() {
        auto uptr1 = std::make_unique<TestObject>();
        TestRef ref1 = uptr1.get();
        BOOST_REQUIRE (ref1.lock() != nullptr);
        BOOST_REQUIRE (ref1.valid());
        uptr1.reset();
        BOOST_REQUIRE (ref1.lock() == nullptr);
        BOOST_REQUIRE (! ref1.valid());
    }

    void subclass() {
        auto uptr1 = std::make_unique<SubObject>();
        TestRef ref1 = uptr1.get();
        BOOST_REQUIRE (ref1.lock() != nullptr);
        BOOST_REQUIRE (ref1.valid());
        BOOST_REQUIRE (ref1.as<SubObject>() != nullptr);
        uptr1.reset();
        BOOST_REQUIRE (ref1.lock() == nullptr);
        BOOST_REQUIRE (! ref1.valid());
        BOOST_REQUIRE (ref1.as<SubObject>() == nullptr);
    }

    class TestObject {
    public:
        TestObject() { weak_status.reset (this); }
        virtual ~TestObject() { weak_status.reset(); }

    private:
        LVTK_WEAK_REFABLE (TestObject, weak_status)
    };

    class SubObject : public TestObject {};

    using TestRef = lvtk::WeakRef<TestObject>;
};

BOOST_AUTO_TEST_SUITE (WeakRef)

BOOST_AUTO_TEST_CASE (basics) {
    WeakRefTest().basics();
}

BOOST_AUTO_TEST_CASE (subclass) {
    WeakRefTest().subclass();
}

BOOST_AUTO_TEST_SUITE_END()
