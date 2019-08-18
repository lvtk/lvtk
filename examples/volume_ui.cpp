
#include <lvtk/ui.hpp>
#include <lvtk/ext/urid.hpp>
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

class VolumeUI : public UIInstance<VolumeUI, URID, Idle, DataAccess, InstanceAccess> {
    uint32_t midi_MidiEvent = 0;

public:
   #if LVTK_STATIC_ARGS
    VolumeUI (const UIArgs& args)
   #else
    VolumeUI (const UIArgs& args)
        : UIInstance (args)
   #endif
    {
        midi_MidiEvent = map ("http://lv2plug.in/ns/ext/midi#MidiEvent");
        if (auto* const instance = plugin_instance())
            std::clog << "VolumeUI got the plugin instance\n";
        if (const void* data = plugin_extension_data (LV2_URID__map))
            std::clog << "VolumeUI got " << LV2_URID__map << " extension data\n";
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

static const UI<VolumeUI> volume_ui ("http://lvtoolkit.org/plugins/volume#ui");
