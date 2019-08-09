#include <lvtk/ui.hpp>
using namespace lvtk;
using lvtk::ui::UI;
using lvtk::ui::Instance;
using lvtk::ui::InstanceArgs;

class VolumeUI : public Instance<VolumeUI> {
public:
    VolumeUI (const InstanceArgs& args)
        : Instance (args)
    {

    }
};

static const UI<VolumeUI> volume_ui ("http://lvtoolkit.org/plugins/volume#ui");
