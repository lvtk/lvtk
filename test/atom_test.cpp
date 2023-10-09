// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <boost/test/unit_test.hpp>

#include <lvtk/ext/atom.hpp>
#include <lvtk/symbols.hpp>

#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/midi/midi.h>
#include <lv2/urid/urid.h>

#include <cstdint>
#include <cstring>
#include <memory>

class AtomTest {
public:
    AtomTest() {
        buffer.reset (new uint8_t[buffer_size]);
        clear_buffer();
        forge.init ((LV2_URID_Map*) urids.map_feature()->data);
    }

    void run_atom() {
        for (uint32_t sz = 2; sz < 2048; sz *= 2)
            BOOST_REQUIRE_EQUAL (lvtk::Atom::pad_size (sz), lv2_atom_pad_size (sz));

        lvtk::Atom atom;
        BOOST_REQUIRE_EQUAL (atom.is_null(), true);
        BOOST_REQUIRE_EQUAL ((bool) atom, false);
        BOOST_REQUIRE_EQUAL (atom.c_obj(), (const LV2_Atom*) nullptr);

        const LV2_Atom_Float afloat = { { sizeof (float), urids.map (LV2_ATOM__Float) }, 100.f };
        atom                        = lvtk::Atom ((void*) &afloat);
        BOOST_ASSERT (atom.has_type_and_equals (urids.map (LV2_ATOM__Float), 100.f));
        BOOST_ASSERT (atom.total_size() == sizeof (float) + sizeof (*atom.c_obj()));
        BOOST_ASSERT (atom.size() == sizeof (float));
        BOOST_ASSERT (atom.type() == urids.map (LV2_ATOM__Float));

        const LV2_Atom_Float bfloat = { { sizeof (float), urids.map (LV2_ATOM__Float) }, 101.f };
        lvtk::Atom batom ((void*) &bfloat);
        BOOST_ASSERT (atom != batom);
        batom = atom;
        BOOST_ASSERT (atom == batom);
    }

    void run_sequence() {
        clear_buffer();
        auto* const cseq = buffer_as<LV2_Atom_Sequence>();
        cseq->atom.type  = urids.map (LV2_ATOM__Sequence);
        cseq->atom.size  = sizeof (LV2_Atom_Sequence_Body);
        cseq->body.unit  = urids.map (LV2_ATOM__frameTime);
        cseq->body.pad   = 0;

        lvtk::Sequence seq (cseq);
        BOOST_ASSERT (seq.size() == sizeof (LV2_Atom_Sequence_Body));
        BOOST_ASSERT (seq.unit() == urids.map (LV2_ATOM__frameTime));

        auto evbuf                   = std::malloc (sizeof (lvtk::AtomEvent) + 3);
        lvtk::AtomEvent* const event = reinterpret_cast<lvtk::AtomEvent*> (evbuf);
        event->time.frames           = 0;
        event->body.type             = urids.map (LV2_MIDI__MidiEvent);
        event->body.size             = 3;
        // note on
        ((uint8_t*) LV2_ATOM_BODY (&event->body))[0] = 0x80;
        ((uint8_t*) LV2_ATOM_BODY (&event->body))[1] = 0x7f;
        ((uint8_t*) LV2_ATOM_BODY (&event->body))[2] = 0x40;

        const uint32_t nev = 20;
        for (uint32_t i = 0; i < nev; ++i) {
            seq.append (*event);
            event->time.frames += 20;
        }

        BOOST_REQUIRE_EQUAL (seq.size(),
                             (nev * lv2_atom_pad_size ((uint32_t) sizeof (lvtk::AtomEvent) + 3)) + (uint32_t) sizeof (LV2_Atom_Sequence_Body));

        uint32_t cnt  = 0;
        int64_t frame = 0;
        for (const auto& ev : seq) {
            BOOST_REQUIRE_EQUAL (urids.map (LV2_MIDI__MidiEvent), ev.body.type);
            BOOST_REQUIRE_EQUAL ((uint32_t) 3, ev.body.size);
            BOOST_REQUIRE_EQUAL (frame, ev.time.frames);
            frame += 20;
            ++cnt;
        }

        BOOST_REQUIRE_EQUAL (nev, cnt);

        // same test but with LV2 macro
        cnt   = 0;
        frame = 0;
        LV2_ATOM_SEQUENCE_FOREACH (seq.c_obj(), iter) {
            auto& ev = *iter;
            BOOST_REQUIRE_EQUAL (urids.map (LV2_MIDI__MidiEvent), ev.body.type);
            BOOST_REQUIRE_EQUAL ((uint32_t) 3, ev.body.size);
            BOOST_REQUIRE_EQUAL (frame, ev.time.frames);
            frame += 20;
            ++cnt;
        }

        BOOST_REQUIRE_EQUAL (nev, cnt);

        event->body.size   = 3;
        event->body.type   = 111;
        event->time.frames = 10;
        seq.insert (*event);
        cnt   = 0;
        frame = 0;
        for (const auto& ev : seq) {
            if (cnt == 1) {
                BOOST_REQUIRE_EQUAL ((uint32_t) 111, ev.body.type);
                BOOST_REQUIRE_EQUAL ((int64_t) 10, ev.time.frames);
                frame += 10;
            } else {
                BOOST_REQUIRE_EQUAL (urids.map (LV2_MIDI__MidiEvent), ev.body.type);
                BOOST_REQUIRE_EQUAL ((uint32_t) 3, ev.body.size);
                BOOST_REQUIRE_EQUAL (frame, ev.time.frames);
                frame += cnt == 0 ? 10 : 20;
            }
            ++cnt;
        }

        BOOST_REQUIRE_EQUAL (nev + 1, cnt);

        std::free (evbuf);
    }

private:
    lvtk::Symbols urids;
    lvtk::Forge forge;
    std::unique_ptr<uint8_t[]> buffer;
    uint32_t buffer_size = 4096 * 2;

    void clear_buffer() { memset (buffer.get(), 0, (size_t) buffer_size); }
    template <class T>
    T* buffer_as() const { return (T*) buffer.get(); }
};

using namespace lvtk;

BOOST_AUTO_TEST_SUITE (Atom)

BOOST_AUTO_TEST_CASE (wrapper) {
    AtomTest().run_atom();
}

BOOST_AUTO_TEST_CASE (sequence_wrapper) {
    AtomTest().run_sequence();
}

BOOST_AUTO_TEST_SUITE_END()
