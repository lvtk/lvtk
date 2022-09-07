
#include "lvtk/weak_ref.hpp"
#include "tests.hpp"
#include <memory>

class WeakRefTest : public TestFixutre {
    CPPUNIT_TEST_SUITE (WeakRefTest);
    CPPUNIT_TEST (basics);
    CPPUNIT_TEST (subclass);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {}

protected:
    void basics() {
        auto uptr1 = std::make_unique<TestObject>();
        TestRef ref1 = uptr1.get();
        CPPUNIT_ASSERT (ref1.lock() != nullptr);
        CPPUNIT_ASSERT (ref1.valid());
        uptr1.reset();
        CPPUNIT_ASSERT (ref1.lock() == nullptr);
        CPPUNIT_ASSERT (! ref1.valid());
    }

    void subclass() {
        auto uptr1 = std::make_unique<SubObject>();
        TestRef ref1 = uptr1.get();
        CPPUNIT_ASSERT (ref1.lock() != nullptr);
        CPPUNIT_ASSERT (ref1.valid());
        CPPUNIT_ASSERT (ref1.as<SubObject>() != nullptr);
        uptr1.reset();
        CPPUNIT_ASSERT (ref1.lock() == nullptr);
        CPPUNIT_ASSERT (! ref1.valid());
        CPPUNIT_ASSERT (ref1.as<SubObject>() == nullptr);
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

CPPUNIT_TEST_SUITE_REGISTRATION (WeakRefTest);
