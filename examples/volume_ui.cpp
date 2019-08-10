
#include <lvtk/ui.hpp>
#include <juce/gui_basics.h>

using namespace lvtk::ui;
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

class VolumeUI : public Instance<VolumeUI> {
public:
    VolumeUI (const InstanceArgs& args)
        : Instance (args)
    {

    }

    void cleanup() {
        widget.reset();
    }

    LV2UI_Widget get_widget()
    {
        if (! widget)
            widget.reset (new VolumeComponent());
        return (LV2UI_Widget) widget.get();
    }

private:
    std::unique_ptr<VolumeComponent> widget;
};

static const UI<VolumeUI> volume_ui ("http://lvtoolkit.org/plugins/volume#ui");
