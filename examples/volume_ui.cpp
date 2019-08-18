
#include <lvtk/ui.hpp>
#include <juce/gui_basics.h>

using namespace lvtk;
using namespace juce;

class VolumeComponent : public Component {
public:
    VolumeComponent()
    {
        setOpaque (true);
        setSize (300, 220);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);
        g.setColour (Colours::black);
        g.drawText ("Volume", getLocalBounds().toFloat(), Justification::centred);
    }

    void resized() override {
        // noop
    }
};

class VolumeUI : public ui::Instance<VolumeUI, Idle, DataAccess, InstanceAccess> {
public:
    VolumeUI (const ui::InstanceArgs& args)
        : Instance (args)
    {
        if (auto* const instance = plugin_instance())
            std::clog << "VolumeUI got the plugin instance\n";
        if (const void* data = plugin_extension_data ("http://dummy.org/ext/data"))
            std::clog << "VolumeUI got extension data\n";
    }

    void cleanup() {
        widget.reset();
    }

    LV2UI_Widget get_widget() {
        if (! widget)
            widget.reset (new VolumeComponent());
        return (LV2UI_Widget) widget.get();
    }

private:
    std::unique_ptr<VolumeComponent> widget;
};

static const ui::UI<VolumeUI> volume_ui ("http://lvtoolkit.org/plugins/volume#ui");
