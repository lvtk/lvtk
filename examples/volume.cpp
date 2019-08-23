
#include <math.h>
#include <lvtk/plugin.hpp>
#include <lvtk/ext/urid.hpp>
#include <lvtk/ext/log.hpp>

#define LVTK_VOLUME_URI "http://lvtoolkit.org/plugins/volume"

using namespace lvtk;

class Volume : public Plugin<Volume, URID, Log>
{
public:
    Volume (const Args& args) : Plugin (args) { }
    
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
        const float gain = *db > -90.0f ? powf (10.0f, *db * 0.05f) : 0.0f;
        for (uint32_t f = 0; f < nframes; ++f)
            for (uint32_t c = 0; c < 2; ++c)
                output[c][f] = input[c][f] * gain;
    }

private:
    float* input[2] { 0, 0 };
    float* output[2] { 0, 0 };
    float* db = nullptr;
};

static const Descriptor<Volume> volume (LVTK_VOLUME_URI, {
    LV2_URID__map // << required host feature
});
