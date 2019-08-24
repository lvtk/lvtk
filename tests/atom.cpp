

#include <memory>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include "tests.hpp"

class Atom : public TestFixutre
{
  CPPUNIT_TEST_SUITE (Atom);
  CPPUNIT_TEST (atom);
  CPPUNIT_TEST (sequence);
  CPPUNIT_TEST_SUITE_END();

protected:
    lvtk::URIDirectory urids;
    lvtk::Forge forge;
    std::unique_ptr<uint8_t[]> buffer;
    uint32_t buffer_size = 4096 * 2;
public:
    void setUp()
    {
        buffer.reset (new uint8_t [buffer_size]);
        clear_buffer();
        forge.init ((LV2_URID_Map*) urids.get_map_feature()->data);
    }

    void clear_buffer() {
        memset (buffer.get(), 0, (size_t) buffer_size);
    }

    template<class T>
    T* buffer_as() const { return (T*) buffer.get(); }

protected:
    void atom() {
        for (uint32_t sz = 2; sz < 2048; sz *= 2)
            CPPUNIT_ASSERT_EQUAL (lvtk::Atom::pad_size (sz), lv2_atom_pad_size (sz));

        lvtk::Atom atom;
        CPPUNIT_ASSERT_EQUAL (atom.is_null(), true);
        CPPUNIT_ASSERT_EQUAL ((bool) atom, false);
        CPPUNIT_ASSERT_EQUAL (atom.c_obj(), (const LV2_Atom*) nullptr);

        const LV2_Atom_Float afloat = { { sizeof(float), urids.map(LV2_ATOM__Float) }, 100.f };
        atom = lvtk::Atom ((void*) &afloat);
        CPPUNIT_ASSERT (atom.has_type_and_equals (urids.map(LV2_ATOM__Float), 100.f));
        CPPUNIT_ASSERT (atom.total_size() == sizeof(float) + sizeof (*atom.c_obj()));
        CPPUNIT_ASSERT (atom.size() == sizeof (float));
        CPPUNIT_ASSERT (atom.type() == urids.map(LV2_ATOM__Float));

        const LV2_Atom_Float bfloat = { { sizeof(float), urids.map(LV2_ATOM__Float) }, 101.f };
        lvtk::Atom batom ((void*) &bfloat);
        CPPUNIT_ASSERT (atom != batom);
        batom = atom;
        CPPUNIT_ASSERT (atom == batom);
    }

    void sequence() {
        clear_buffer();
        auto* const cseq = buffer_as<LV2_Atom_Sequence>();
        cseq->atom.type  = urids.map (LV2_ATOM__Sequence);
        cseq->atom.size  = sizeof (LV2_Atom_Sequence_Body);
        cseq->body.unit  = urids.map (LV2_ATOM__frameTime);
        cseq->body.pad   = 0;

        lvtk::Sequence seq (cseq);
        CPPUNIT_ASSERT (seq.size() == sizeof (LV2_Atom_Sequence_Body));
        CPPUNIT_ASSERT (seq.unit() == urids.map (LV2_ATOM__frameTime));

        uint8_t evbuf [sizeof (lvtk::AtomEvent) + 3];
        lvtk::AtomEvent* const event = (lvtk::AtomEvent*) evbuf;
        event->time.frames = 0;
        event->body.type = urids.map (LV2_MIDI__MidiEvent);
        event->body.size = 3;
        // note on
        ((uint8_t*)LV2_ATOM_BODY (&event->body))[0] = 0x80;
        ((uint8_t*)LV2_ATOM_BODY (&event->body))[1] = 0x7f;
        ((uint8_t*)LV2_ATOM_BODY (&event->body))[2] = 0x40;
        
        const uint32_t nev = 20;
        for (uint32_t i = 0; i < nev; ++i) {
            seq.append (*event);
            event->time.frames += 20;
        }

        CPPUNIT_ASSERT_EQUAL (seq.size(),
            (nev * lv2_atom_pad_size ((uint32_t) sizeof (lvtk::AtomEvent) + 3)) + (uint32_t)sizeof (LV2_Atom_Sequence_Body));

        uint32_t cnt = 0;
        int64_t frame = 0;
        for (const auto& ev : seq) {
            CPPUNIT_ASSERT_EQUAL (urids.map (LV2_MIDI__MidiEvent), ev.body.type);
            CPPUNIT_ASSERT_EQUAL ((uint32_t) 3, ev.body.size);
            CPPUNIT_ASSERT_EQUAL (frame, ev.time.frames);
            frame += 20;
            ++cnt;
        }

        CPPUNIT_ASSERT_EQUAL (nev, cnt);

        // same test but with LV2 macro
        cnt = 0;
        frame = 0;
        LV2_ATOM_SEQUENCE_FOREACH (seq.c_obj(), iter) {
            auto& ev = *iter;
            CPPUNIT_ASSERT_EQUAL (urids.map (LV2_MIDI__MidiEvent), ev.body.type);
            CPPUNIT_ASSERT_EQUAL ((uint32_t)3, ev.body.size);
            CPPUNIT_ASSERT_EQUAL (frame, ev.time.frames);
            frame += 20;
            ++cnt;
        }

        CPPUNIT_ASSERT_EQUAL (nev, cnt);
        
        event->body.size = 3;
        event->body.type = 111;
        event->time.frames = 10;
        seq.insert (*event);
        cnt = 0;
        frame = 0;
        for (const auto& ev : seq) {
            if (cnt == 1) {
                CPPUNIT_ASSERT_EQUAL ((uint32_t) 111, ev.body.type);
                CPPUNIT_ASSERT_EQUAL ((int64_t) 10, ev.time.frames);
                frame += 10;
            } else {
                CPPUNIT_ASSERT_EQUAL (urids.map (LV2_MIDI__MidiEvent), ev.body.type);
                CPPUNIT_ASSERT_EQUAL ((uint32_t)3, ev.body.size);
                CPPUNIT_ASSERT_EQUAL (frame, ev.time.frames);
                frame += cnt == 0 ? 10 : 20;
            }
            ++cnt;
        }

        CPPUNIT_ASSERT_EQUAL (nev + 1, cnt);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (Atom);
