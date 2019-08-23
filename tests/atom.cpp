
#include "tests.hpp"

class Atom : public TestFixutre
{
  CPPUNIT_TEST_SUITE (Atom);
  CPPUNIT_TEST (atom);
  CPPUNIT_TEST_SUITE_END();

protected:
    lvtk::URIDirectory urids;

public:
    void setUp()
    {
        
    }

protected:
    void atom() {
        for (uint32_t sz = 2; sz < 2048; sz *= 2)
            CPPUNIT_ASSERT_EQUAL (lvtk::Atom::pad_size (sz), lv2_atom_pad_size (sz));

        lvtk::Atom atom;
        CPPUNIT_ASSERT_EQUAL (atom.is_null(), true);
        CPPUNIT_ASSERT_EQUAL ((bool) atom, false);
        CPPUNIT_ASSERT_EQUAL (atom.c_obj(), (const LV2_Atom*) nullptr);

        const LV2_Atom_Float afloat = { { sizeof(float), 1 }, 100.f };
        atom = lvtk::Atom ((void*) &afloat);
        CPPUNIT_ASSERT (atom.has_type_and_equals (1, 100.f));
        CPPUNIT_ASSERT (atom.total_size() == sizeof(float) + sizeof (*atom.c_obj()));
        CPPUNIT_ASSERT (atom.size() == sizeof (float));
        CPPUNIT_ASSERT (atom.type() == 1);

        const LV2_Atom_Float bfloat = { { sizeof(float), 1 }, 101.f };
        lvtk::Atom batom ((void*) &bfloat);
        CPPUNIT_ASSERT (atom != batom);
        batom = atom;
        CPPUNIT_ASSERT (atom == batom);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (Atom);
