/* 
    Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/** @file volume.cpp */

#include <math.h>
#include <lvtk/plugin.hpp>
#include <lvtk/ext/urid.hpp>

#define LVTK_VOLUME_URI "http://lvtoolkit.org/plugins/volume"

namespace lvtk {

class Volume : public Instance<Volume, URID>
{
public:
    Volume (const Args& args) : Instance (args) { }

    void activate() { }
    void deactivate() { }

    void connect_port (uint32_t port, void* data) {
        if (port == 0)
            input[0] = (float*) data;
        else if (port == 1)
            input[1] = (float*) data;
        else if (port == 2)
            output[0] = (float*) data;
        else if (port == 3)
            output[1] = (float*) data;
        else if (port == 4 && data != nullptr)
            db = *(float*) data;
    }

    void run (uint32_t nframes) { 
        const float gain = db > -90.0f ? powf (10.0f, db * 0.05f) : 0.0f;
        for (uint32_t f = 0; f < nframes; ++f)
            for (uint32_t c = 0; c < 2; ++c)
                output[c][f] = input[c][f] * gain;
    }

private:
    float* input[2] { 0, 0 };
    float* output[2] { 0, 0 };
    float db = 0.0;
};

static Plugin<Volume> volume (LVTK_VOLUME_URI, {
    LV2_URID__map ///* < required host feature
});

}
