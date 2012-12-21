/*

    synth.hpp - Support file for writing LV2 tone generating plugins in C++

    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

 */
/** @headerfile lvtk/synth.hpp */

#ifndef LVTK_LV2_SYNTH_HPP
#define LVTK_LV2_SYNTH_HPP

#include <cmath>
#include <cstring>
#include <vector>

#include <lvtk/plugin.hpp>

namespace lvtk {

    using std::vector;

    /** This is used as an "invalid" MIDI key number, meaning "no key". */
    static const unsigned char INVALID_KEY = 255;


    /** A simple function that translates a MIDI key number to a fundamental
      frequency in Hz. */
    static inline float key2hz(unsigned char key) {
        return 8.1758 * std::pow(1.0594, key);
    }


    /** A base class for synth voices, to be used with the Synth template class.
      You don't have to make your voice classes inherit this one, but it
      makes some things easier.
     */
    class Voice {
    public:

        /** Turn the voice on. This default implementation does nothing, you
        probably want to override it.

        If @c key is lvtk::INVALID_KEY the voice should go silent as fast at
        possible (the synth may use this when it receives an All Sound Off
        event).
        @param key The MIDI key for the note that the voice should play.
        @param velocity The MIDI velocity for the Note On event.
         */
        void on(unsigned char key, unsigned char velocity) { }

        /** Turn the voice off. This default implementation does nothing, you
        probably want to override it.
        @param velocity The MIDI velocity for the Note Off event.
         */
        void off(unsigned char velocity) { }

        /** Return the MIDI key that the voice is currently playing.
        lvtk::INVALID_KEY means that the voice is not active and could be used
        to play a new note. */
        unsigned char get_key() const { return lvtk::INVALID_KEY; }

        /** Render audio for this voice to the output buffers, from sample
        @c from to sample @c to. The buffers may already contain audio from
        other voices, so use += instead of = when writing to it. This default
        implementation does nothing, you probably want to override it.
         */
        void render(uint32_t from, uint32_t to) { }

        /** @internal
        Set the port buffer vector. The Synth class always calls this before
        it calls render().
         */
        void set_port_buffers(vector<void*>& ports) { m_ports = &ports; }

    protected:

        /** Same as Plugin::p() - returns the buffer for the given port.
         */
        template <typename T> inline T*& p(uint32_t port) {
            return reinterpret_cast<T*&>((*m_ports)[port]);
        }

        /** Same as Plugin::p() - returns the buffer for the given port.
         */
        float*& p(uint32_t port) {
            return reinterpret_cast<float*&>((*m_ports)[port]);
        }

        /** @internal
        The current port buffer vector. */
        vector<void*>* m_ports;
    };


    /** This is a base class template for LV2 synth plugins. Its parameters
        are a voice class, either a subclass of Voice or something written from
        scratch, and the derived class itself. By using the derived class as a
        template parameter the base class can call member functions in the
        derived class without resorting to virtual function calls, which are
        hard to optimise.

        You can use @ref pluginmixins "mixins" with this class just like with
        the Plugin class, but don't use URID - they are already
        added automatically.

        Here is an example of a complete synth plugin. Granted, not a very
        exciting one, but it should be enough to explain how to do it:

        @code
#include <cstdlib>
#include <lvtk/synth.hpp>

enum {
  MIDI_PORT,
  AUDIO_PORT,
  NUM_PORTS
};

struct NoiseVoice : public lvtk::Voice {

  NoiseVoice() : m_gain(0), m_key(INVALID_KEY) { }

  void on(unsigned char k, unsigned char velocity) { 
    m_gain = 1.0; 
    m_key = k;
  }

  void off(unsigned char velocity) { 
    m_gain = 0.0; 
    m_key = lvtk::INVALID_KEY;
  }

  unsigned char get_key() const { 
    return m_key; 
  }

  void render(uint32_t from, uint32_t to) {
    for (uint32_t i = from; i < to; ++i) {
      p(AUDIO_PORT)[i] += m_gain * 2 * (rand() / float(RAND_MAX) - 0.5);
      m_gain *= 0.9999;
    }
  }

  float m_gain;
  unsigned char m_key;
};


struct NoiseSynth : public lvtk::Synth<NoiseVoice, NoiseSynth> {

  NoiseSynth(double) 
    : lvtk::Synth<NoiseVoice, NoiseSynth>(NUM_PORTS, MIDI_PORT), m_filterstate(0) {
    add_voices(new NoiseVoice, new NoiseVoice, new NoiseVoice);
    add_audio_outputs(AUDIO_PORT);
  }

  void post_process(uint32_t from, uint32_t to) {
    for (uint32_t i = from; i < to; ++i) {
      p(AUDIO_PORT)[i] = m_filterstate * 0.9 + p(AUDIO_PORT)[i] * 0.1;
      m_filterstate = p(AUDIO_PORT)[i];
    }
  }

  float m_filterstate;
};
      @endcode
     */
    template <class V, class D,
              class Ext1 = end, class Ext2 = end, class Ext3 = end,
              class Ext4 = end, class Ext5 = end, class Ext6 = end,
              class Ext7 = end>
    class Synth : public Plugin<D, URID<true>, Ext1, Ext2, Ext3, Ext4, Ext5, Ext6, Ext7>
    {
    public:

        /** @internal
	Convenient typedef for the parent class. */
        typedef Plugin<D, URID<true>, Ext1, Ext2, Ext3, Ext4, Ext5, Ext6, Ext7>
        Parent;


        /** This constructor must be called in the initialiser list for your
            synth class.
            @param ports The total number of ports in your plugin.
            @param midi_input The index of the main MIDI input port (the one that
            the synth should use for note input).
         */
        Synth(uint32_t ports, uint32_t midi_input)
        : Parent(ports),
          m_midi_input (midi_input)
        {
            m_midi_type = Parent::map(LV2_MIDI__MidiEvent);
        }


        /** This is needed to delete the voices.
         */
        ~Synth()
        {
            for (unsigned i = 0; i < m_voices.size(); ++i)
                delete m_voices[i];
        }


        /** This function implements the voice stealing algorithm. The @c key and
            @c velocity arguments are the parameters for the MIDI Note On event
            that needs a voice. This default implementation just returns the
            first free voice, and if there is none it steals voice 0.

            This is not a virtual function, but if you override it in a subclass
            this class will still use that implementation thanks to the second
            template class parameter. This means that you can override this function
            if you want to implement your own voice stealing algorithm. */
        unsigned
        find_free_voice(unsigned char key, unsigned char velocity)
        {
            for (unsigned i = 0; i < m_voices.size(); ++i)
            {
                if (m_voices[i]->get_key() == lvtk::INVALID_KEY)
                    return i;
            }

            return 0;
        }


        /** This function is called for every MIDI event in the input. It will
            respond to note on and note off events by calling on() and off() in
            the voices.

            This is not a virtual function, but if you override it in a subclass
            this class will still use that implementation thanks to the second
            template class parameter. This means that you can override this function
            if you want to respond to other MIDI events - just be sure to either
            call this function for note on and note off events or turn on and off
            the voices yourself. */
        void
        handle_midi (uint32_t size, unsigned char* data)
        {
            if (size != 3) return;

            if (data[0] == 0x90)
            {
                unsigned voice =
                        static_cast<D*>(this)->find_free_voice(data[1], data[2]);
                if (voice < m_voices.size())
                    m_voices[voice]->on(data[1], data[2]);
            }
            else if (data[0] == 0x80)
            {
                for (unsigned i = 0; i < m_voices.size(); ++i)
                {
                    if (m_voices[i]->get_key() == data[1])
                    {
                        m_voices[i]->off(data[2]);
                        break;
                    }
                }
            }
        }


        /** This function is called before the synth renders a chunk of audio from
            the voices, from sample @c from to sample @c to in the output buffers.
            It can be used to compute parameters for the voices or adding initial
            ambient noise to the output buffers or something else. This default
            implementation does nothing.

            This is not a virtual function, but if you override it in a subclass
            this class will still use that implementation thanks to the second
            template class parameter. This means that you can override this function
            if you want to implement your own pre-processing.
         */
        void
        pre_process(uint32_t from, uint32_t to)
        { }


        /** This function is called after the synth renders a chunk of audio from
            the voices, from sample @c from to sample @c to in the output buffers.
            It can be used to apply global effects to the mixed audio. This default
            implementation does nothing.

            This is not a virtual function, but if you override it in a subclass
            this class will still use that implementation thanks to the second
            template class parameter. This means that you can override this function
            if you want to implement your own post-processing.
         */
        void
        post_process(uint32_t from, uint32_t to)
        {  }


        /** This is the main run function. It handles incoming MIDI events and
            mixes the voices to the output buffers with pre and post-processing
            applied. Don't override it unless you really know what you're doing.
        */
        void
        run(uint32_t sample_count)
        {
            D* synth = static_cast<D*>(this);

            // Zero output buffers so voices can add to them
            for (unsigned i = 0; i < m_audio_ports.size(); ++i)
                std::memset(p(m_audio_ports[i]), 0, sizeof(float) * sample_count);

            // Make the port buffers available to the voices
            for (unsigned i = 0; i < m_voices.size(); ++i)
                m_voices[i]->set_port_buffers(Parent::m_ports);

            // Get ahold of the first atom event if any
            const LV2_Atom_Sequence* seq = p<LV2_Atom_Sequence> (m_midi_input);
            LV2_Atom_Event* ev = lv2_atom_sequence_begin(&seq->body);

            uint32_t samples_done = 0;

            while (samples_done < sample_count)
            {
                uint32_t to = sample_count;

                if (lv2_atom_sequence_is_end(&seq->body, seq->atom.size, ev))
                {
                    ev = NULL;
                }
                else
                {
                    to = ev->time.frames;
                }

                if (to > samples_done)
                {
                    synth->pre_process(samples_done, to);
                    for (unsigned i = 0; i < m_voices.size(); ++i)
                        m_voices[i]->render(samples_done, to);
                    synth->post_process(samples_done, to);
                    samples_done = to;
                }

                if (ev)
                {
                    if (ev->body.type == m_midi_type)
                    {
                        uint8_t* const data = (uint8_t* const)(ev + 1);
                        handle_midi (ev->body.size, data);
                    }

                    ev = lv2_atom_sequence_next (ev);
                }
            }
        }


        /** This should be called in the constructor of your derived class
            to tell the synth which ports the voices are using as audio output
            buffers, so the synth engine can fill them with zeros before rendering
            the voice audio.

            If you for some reason don't want to zero your audio output buffers
            before the voices get mixed (you may for example want to fill them with
            something else in pre_process()) you should not	call this function -
            but then you MUST take care of initialising the buffers yourself in
            every cycle.
         */
        void
        add_audio_outputs(uint32_t p1 = -1, uint32_t p2 = -1,
                          uint32_t p3 = -1, uint32_t p4 = -1,
                          uint32_t p5 = -1, uint32_t p6 = -1)
        {
            if (p1 == uint32_t(-1))
                return;
            m_audio_ports.push_back(p1);
            if (p2 == uint32_t(-1))
                return;
            m_audio_ports.push_back(p2);
            if (p3 == uint32_t(-1))
                return;
            m_audio_ports.push_back(p3);
            if (p4 == uint32_t(-1))
                return;
            m_audio_ports.push_back(p4);
            if (p5 == uint32_t(-1))
                return;
            m_audio_ports.push_back(p5);
            if (p6 == uint32_t(-1))
                return;
            m_audio_ports.push_back(p6);
        }


        /** This is typically called in the constructor of your derived class
            to add voices to the synth. The number of voices you add determine the
            polyphony. It can also be called while the plugin is running to change
            the polyphony on the fly, but it is NOT realtime safe and NOT
            threadsafe - you have to make sure that the run() callback isn't
            executing simultaneously with this function.

            lvtk::Synth will assume ownership of the voices and delete them in its
            destructor.
        */
        void add_voices(V* v01 = 0, V* v02 = 0, V* v03 = 0, V* v04 = 0, V* v05 = 0,
                        V* v06 = 0, V* v07 = 0, V* v08 = 0, V* v09 = 0, V* v10 = 0,
                        V* v11 = 0, V* v12 = 0, V* v13 = 0, V* v14 = 0, V* v15 = 0,
                        V* v16 = 0, V* v17 = 0, V* v18 = 0, V* v19 = 0, V* v20 = 0)
        {
            if (v01 == 0)
                return;
            m_voices.push_back(v01);
            if (v02 == 0)
                return;
            m_voices.push_back(v02);
            if (v03 == 0)
                return;
            m_voices.push_back(v03);
            if (v04 == 0)
                return;
            m_voices.push_back(v04);
            if (v05 == 0)
                return;
            m_voices.push_back(v05);
            if (v06 == 0)
                return;
            m_voices.push_back(v06);
            if (v07 == 0)
                return;
            m_voices.push_back(v07);
            if (v08 == 0)
                return;
            m_voices.push_back(v08);
            if (v09 == 0)
                return;
            m_voices.push_back(v09);
            if (v10 == 0)
                return;
            m_voices.push_back(v10);
            if (v11 == 0)
                return;
            m_voices.push_back(v11);
            if (v12 == 0)
                return;
            m_voices.push_back(v12);
            if (v13 == 0)
                return;
            m_voices.push_back(v13);
            if (v14 == 0)
                return;
            m_voices.push_back(v14);
            if (v15 == 0)
                return;
            m_voices.push_back(v15);
            if (v16 == 0)
                return;
            m_voices.push_back(v16);
            if (v17 == 0)
                return;
            m_voices.push_back(v17);
            if (v18 == 0)
                return;
            m_voices.push_back(v18);
            if (v19 == 0)
                return;
            m_voices.push_back(v19);
            if (v20 == 0)
                return;
            m_voices.push_back(v20);
        }
    protected:

        /** Use this function to access data buffers for ports. They will be
            casted to pointers to the template parameter @c T. */
        template <typename T> T*&
        p(uint32_t port) {
            return reinterpret_cast<T*&>(Parent::m_ports[port]);
        }

        /** Use this function to access data buffers for control or audio ports. */
        float*&
        p(uint32_t port) {
            return reinterpret_cast<float*&>(Parent::m_ports[port]);
        }


        /** @internal The voice objects that render the audio. */
        vector<V*> m_voices;

        /** @internal The indices of all audio output ports. */
        vector<uint32_t> m_audio_ports;

        /** @internal The index of the main MIDI input port. */
        uint32_t m_midi_input;

        /** @internal The numerical ID for the MIDI event type. */
        LV2_URID m_midi_type;

    };

}


#endif
