// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "lvtk/lvtk.hpp"
#include "lvtk/plugin.hpp"

#include <cmath>
#include <cstdint>
#include <vector>

#define LVTK_VOLUME_URI "https://lvtk.org/plugins/volume"

class Volume : public lvtk::Plugin<Volume> {
public:
    Volume (const lvtk::Args& args) : Plugin (args) {
        lpf = 990.f / static_cast<float> (args.sample_rate);
    }

    void connect_port (uint32_t port, void* data) {
        if (port == 0)
            input[0] = (float*) data;
        else if (port == 1)
            input[1] = (float*) data;
        else if (port == 2)
            output[0] = (float*) data;
        else if (port == 3)
            output[1] = (float*) data;
        else if (port == 4)
            db = (float*) data;
    }

    void run (uint32_t nframes) {
        gains.next = *db > -90.0f ? powf (10.0f, *db * 0.05f) : 0.0f;

        if (fabsf (gains.last - gains.next) < 0.01) {
            // constant gain
            for (uint32_t c = 0; c < 2; ++c)
                for (uint32_t f = 0; f < nframes; ++f)
                    output[c][f] = input[c][f] * gains.next;
            gains.last = gains.next;
        } else {
            // smoothed gain
            float gain     = gains.last;
            uint32_t begin = 0;

            while (begin < nframes) {
                uint32_t remain = nframes - begin;
                uint32_t todo   = remain > 16 ? 16 : remain;
                gain += lpf * (gains.next - gain);
                for (uint32_t c = 0; c < 2; ++c)
                    for (uint32_t f = begin; f < begin + todo; ++f)
                        output[c][f] = input[c][f] * gain;
                begin += todo;
            }

            gains.last = gain;
        }
    }

private:
    float* input[2] { 0, 0 };
    float* output[2] { 0, 0 };
    float* db = nullptr;
    float lpf = 0.f;

    struct Gains {
        float last = 0.f;
        float next = 1.f;
    } gains;
};

static const lvtk::Descriptor<Volume> volume (LVTK_VOLUME_URI);
