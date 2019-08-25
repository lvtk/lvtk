
#include <lvtk/ui.hpp>
#include <lvtk/ext/urid.hpp>
#include <lvtk/ext/data_access.hpp>
#include <lvtk/ext/instance_access.hpp>
#include <lvtk/ext/ui/resize.hpp>
#include <lvtk/ext/ui/touch.hpp>

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

class VolumeUI : public UI<VolumeUI, DataAccess, InstanceAccess, Resize, Touch> {
public:
    VolumeUI (const UIArgs& args) : UI (args) {
        // The UI base class and all Extensions will have processed the args/features
        // so that their data and features are available in your contstructor.

        // plugin_instance() and plugin_extension_data() below aren't actually used
        // in this plugin.  It is an example of what is added by the DataAccess 
        // and InstanceAccess extensions
        if (auto* const instance = plugin_instance())
            std::clog << "VolumeUI got the plugin instance\n";
        if (const void* data = plugin_extension_data (LV2_URID__map))
            std::clog << "VolumeUI got " << LV2_URID__map << " extension data\n";

        widget.reset (new VolumeComponent());
        
        // notify_size() is added by the Resize extension
        notify_size (widget->getWidth(), widget->getHeight());

        auto& slider = widget->slider;
        
        // The touch() method below is added by the Touch extension
        // How to use ui_touch with std::bind (these could also be lambdas)
        slider.onDragStart = std::bind (&VolumeUI::touch, this, 4, true);
        slider.onDragEnd   = std::bind (&VolumeUI::touch, this, 4, false);

        // Example binding with a lamda
        slider.onValueChange = [this]() {
            write (4, static_cast<float> (widget->slider.getValue()));
        };
    }

    void cleanup() {
        // cleanup implemented to guarantee bindings are cleared before
        // the destructor
        widget.reset();
    }

    void port_event (uint32_t port, uint32_t size, uint32_t protocol, const void* data) {
        if (port == 4 && protocol == 0) {
            // control port changed detected, uptate the slider
            // juce::dontSendNotification prevents from lamda callbacks from being
            // triggered
            widget->slider.setValue (*(float*) data, dontSendNotification);
        }
    }

    LV2UI_Widget get_widget() {
        return (LV2UI_Widget) widget.get();
    }

    // size requested is added by the Resize extension, and is
    // called by the host when it wants to resize your UI
    int size_requested (int width, int height) {
        if (! widget)
            return 1; // non-zero indicates an error
        widget->setSize (width, height);
        return 0; // zero is all good!
    }

private:
    std::unique_ptr<VolumeComponent> widget;
    uint32_t midi_MidiEvent = 0;
};

// UI Descriptors are registered with the UIDescriptor class
// The template param is your UI subclass.  This is used internally
// by UIDescriptor to assign LV2 UI callbacks and add a LV2UI_Descriptor 
// to the global list of UI's
static const UIDescriptor<VolumeUI> volume_ui (LVTK_VOLUME_UI_URI);
