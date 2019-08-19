
#include <lvtk/ui.hpp>
#include <lvtk/ext/urid.hpp>
#include <lvtk/ext/data_access.hpp>
#include <lvtk/ext/instance_access.hpp>

#include <juce/gui_basics.h>

#define LVTK_VOLUME_UI_URI "http://lvtoolkit.org/plugins/volume#ui"

using namespace lvtk;
using namespace juce;

class VolumeComponent : public Component {
public:
    VolumeComponent()
    {
        setOpaque (true);
        addAndMakeVisible (slider);
        slider.setRange (-90.f, 24.f);
        slider.setSliderStyle (Slider::Rotary);
        slider.setTextBoxStyle (Slider::TextBoxBelow, true,
                                slider.getTextBoxWidth(),
                                slider.getTextBoxHeight());
        setSize (220, 220);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (findColour (DocumentWindow::backgroundColourId));
    }

    void resized() override {
        slider.setBounds (getLocalBounds().reduced (6));
    }

    Slider slider;
};

class VolumeUI : public UIInstance<VolumeUI, URID, DataAccess, InstanceAccess> {
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

        widget.reset (new VolumeComponent());
        widget->slider.onValueChange = [this]() {
            write (4, static_cast<float> (widget->slider.getValue()));
        };
    }

    void cleanup() {
        widget.reset();
    }

    void port_event (uint32_t port, uint32_t size, uint32_t protocol, const void* data) {
        if (port == 4 && protocol == 0) {
            widget->slider.setValue (*(float*) data, dontSendNotification);
        }
    }
    
    LV2UI_Widget get_widget() {
        return (LV2UI_Widget) widget.get();
    }

private:
    std::unique_ptr<VolumeComponent> widget;
};

static const UI<VolumeUI> volume_ui (LVTK_VOLUME_UI_URI);
