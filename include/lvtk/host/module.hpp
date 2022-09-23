// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lv2/core/lv2.h>
#include <lv2/ui/ui.h>
#include <lv2/data-access/data-access.h>
#include <lv2/instance-access/instance-access.h>

#include <suil/suil.h>

#include <cstdint>
#include <string>
#include <vector>

#ifdef __linux__
#    define LVTK_UI__NativeUI LV2_UI__X11UI
#elif defined(__APPLE__)
#    define LVTK_UI__NativeUI LV2_UI__CocoaUI
#else
#    define LVTK_UI__NativeUI LV2_UI__WindowsUI
#endif

namespace lvtk {

class World;

/** A UI that is supported for loading */
struct LVTK_API SupportedUI {
    std::string URI;        ///< UI's URI
    std::string container;  ///< Container type URI
    std::string widget;     ///< Widget Type URI
    bool show {false};      ///< Wants show interface
    bool idle {false};      ///< Wants idle interface
    unsigned quality {0};  ///< Quality of the GUI
};

/** A vector of supported UIs */
class LVTK_API SupportedUIs : public std::vector<SupportedUI> {};

/** Details about an LV2 plugin */
struct LVTK_API PluginInfo {
    std::string URI;                ///< Plugin's URI
    std::string name;               ///< Plugin Name
    std::string author_name;        ///< Plugin author
    std::string author_homepage;    ///< Plugin homepage URL
    std::string author_email;       ///< Author's email
    SupportedUIs uis;               ///< UIs supported by the plugin.
};

/** @private */
class InstanceImpl;

/** A LV2 plugin instance.
 
    Powerd by Lilv
*/
class LVTK_API Instance final {
public:
    ~Instance();

    /** Returns this Plugin's name */
    std::string name() const noexcept;

    void activate();
    void deactivate();
    void connect_port (uint32_t port, void* data);
    void run (uint32_t nframes);

private:
    friend class World;
    std::unique_ptr<InstanceImpl> impl;
    Instance();
};

/** @private */
class InstanceUIImpl;

class LVTK_API InstanceUI final {
public:
    ~InstanceUI();

    /** Set this to handle touch notifications from the UI */
    std::function<void(uint32_t, bool)> touched;

    /** Called when the UI invokes LV2UI_Resize::ui_resize 
        Use requestSize to request the UI changes it size from the host
        return zero on success
    */
    std::function<int()> client_resized;

    bool loaded() const { return _instance != nullptr; }

    void unload()
    {
        // module.clearEditor();
        
        if (_instance)
        {
            suil_instance_free (_instance);
            _instance = nullptr;
        }
    }

    World& world() const { return _world; }
    Instance& instance() const { return module; }

    LV2UI_Widget widget() const
    { 
        return _instance != nullptr ? suil_instance_get_widget (_instance)
                                   : nullptr;
    }

    void portEvent (uint32_t port, uint32_t size, uint32_t format, const void* buffer)
    {
        if (_instance == nullptr)
            return;
        suil_instance_port_event (_instance, port, size, format, buffer);
    }

    bool is_native() const { return container_type == LVTK_UI__NativeUI; }

    bool has_container_type (const std::string& type) const
    {
        return container_type == type;
    }

    bool is_a (const std::string& widget_type_uri) const
    {
        return widget_type_uri == widget_type;
    }

    void instantiate()
    {
        if (loaded())
            return;
        
        std::vector<const LV2_Feature*> features;
        // world.getFeatures (features);

        // parent
        if (parent.data != nullptr)
            features.push_back (&parent);
        
        // resize host side
        host_resize_data.handle = this;
        host_resize_data.ui_resize = InstanceUI::_host_resize;
        resize_feature.data = (void*) &host_resize_data;
        features.push_back (&resize_feature);

        // instance access
        // FIXME:
        // if (auto handle = module.handle())
        // {
        //     instanceFeature.data = (void*) handle;
        //     features.add (&instanceFeature);
        // }

        // data access
        data_fdata.data_access = InstanceUI::_data_access;
        data_feature.data = &data_fdata;

        // terminate the array
        features.push_back (nullptr);

        // FIXME: create the instance
        _instance = suil_instance_new (
            nullptr, this,
            container_type.c_str(),
            plugin.c_str(),
            ui.c_str(),
            widget_type.c_str(),
            bundle_path.c_str(),
            binary_path.c_str(),
            features.data()
        );

        // Nullify all UI extensions
        ui_resize = nullptr;
        ui_idle = nullptr;
        ui_show = nullptr;

        if (nullptr == _instance)
            return;

        // resize - plugin side
        if (const auto* resizedata = suil_instance_extension_data (_instance, LV2_UI__resize))
            ui_resize = (LV2UI_Resize*) resizedata;
        
        // Idle Interface
        if (const auto* idledata = suil_instance_extension_data (_instance, LV2_UI__idleInterface))
            ui_idle = (LV2UI_Idle_Interface*) idledata;
        
        // Show Interface
        if (const auto* showdata = suil_instance_extension_data (_instance, LV2_UI__showInterface))
            ui_show = (LV2UI_Show_Interface*) showdata;
    }

    bool have_show_interface() const
    { 
        return _instance != nullptr &&
            ui_show != nullptr && 
            ui_show->show != nullptr &&
            ui_show->hide != nullptr;
    }

    // fixme
    bool requires_show_interface() const { return false; }

    bool show()
    {
        if (! have_show_interface())
            return false;
        return 0 == ui_show->show ((LV2UI_Handle) suil_instance_get_handle (_instance));
    }

    bool hide()
    {
        if (! have_show_interface())
            return false;
        return 0 == ui_show->hide ((LV2UI_Handle) suil_instance_get_handle (_instance));
    }

    /** Returns true if the plugin provided LV2_UI__idleInterface */
    bool have_idle_interface() const { return nullptr != ui_idle && nullptr != _instance; }
    
    void idle()
    {
        if (! have_idle_interface())
            return;
        ui_idle->idle ((LV2UI_Handle) suil_instance_get_handle (_instance));
    }

    void set_parent (uintptr_t ptr) 
    {
        parent.data = (void*) ptr;
    }

    /** returs true if the UI provided LV2_UI__resize */
    bool have_client_resize() const
    {
        return ui_resize != nullptr &&
            ui_resize->handle != nullptr &&
            ui_resize->ui_resize != nullptr;
    }

    /** Request the UI update it's size. This is the Host side of
        LV2_UI__resize
     */
    bool request_size (int width, int height)
    {
        return (have_client_resize()) ? ui_resize->ui_resize (ui_resize->handle, width, height)
                                    : false;
    }

    /** Returns the width as reported by UI-side LV2_UI__resize */
    int width() const noexcept { return client_width; }

    /** Returns the width as reported by UI-side LV2_UI__resize */
    int height() const noexcept { return client_height; }

private:
    friend class Instance;
    friend class World;
    std::unique_ptr<InstanceUIImpl> impl;

    LV2UI_Idle_Interface *ui_idle = nullptr;
    LV2UI_Show_Interface *ui_show = nullptr;
    LV2UI_Resize* ui_resize = nullptr;

    LV2_Feature parent { LV2_UI__parent, nullptr };
    LV2_Feature resize_feature { LV2_UI__resize, nullptr };
    LV2UI_Resize host_resize_data;
    LV2_Feature instanceFeature { LV2_INSTANCE_ACCESS_URI, nullptr };
    LV2_Feature data_feature { LV2_DATA_ACCESS_URI, nullptr };
    LV2_Extension_Data_Feature data_fdata;

    int client_width = 0;
    int client_height = 0;

    InstanceUI (World& w, Instance& m);

    World& _world;
    Instance& module;

    SuilInstance* _instance = nullptr;
    std::string container_type {};
    std::string plugin {};
    std::string ui {};
    std::string widget_type {};
    std::string bundle_path {};
    std::string binary_path {};
    bool requires_show = false;

    static const void* _data_access (const char* uri)
    {
        return nullptr;
    }

    static int _host_resize (LV2UI_Feature_Handle handle, int width, int height)
    {
        auto* ui = static_cast<InstanceUI*> (handle);
        // ui->client_width = width;
        // ui->client_height = height;
        // return (ui->onClientResize) ? ui->onClientResize() : 0;
    }

    static void _port_write (void* controller, uint32_t port, uint32_t size,
                            uint32_t protocol, void const* buffer)
    {
        // auto& plugin = (static_cast<InstanceUI*> (controller))->getPlugin();
        // plugin.write (port, size, protocol, buffer);
    }

    static uint32_t _port_index (void* controller, const char* symbol)
    {
        // auto& plugin = (static_cast<Instance*> (controller))->getPlugin();
        // return plugin.getPortIndex (symbol);
        return 0;
    }

    static uint32_t _subscribe (void* controller, uint32_t port_index, 
                                   uint32_t protocol, const LV2_Feature *const *features)
    {
        return 0;
    }

    static uint32_t _unsubscribe (void* controller, uint32_t port_index, 
                                     uint32_t protocol, const LV2_Feature *const *features)
    {
        return 0;
    }

    static void touch (void* controller, uint32_t port_index, bool grabbed)
    {
        auto* ui = (static_cast<InstanceUI*> (controller));
        if (ui && ui->touched)
            ui->touched (port_index, grabbed);
    }

private:
    InstanceUI();
};

}

#if 0
Skip to content
Product
Solutions
Open Source
Pricing
Search
Sign in
Sign up
This repository has been archived by the owner. It is now read-only.
lvtk
/
jlv2
Public archive
Code
Issues
3
Pull requests
Actions
Security
Insights
jlv2/modules/jlv2_host/host/Module.cpp
@atsushieno
atsushieno Error out if worker feature is null and an extension data is Worker i…
…
Latest commit 44ec197 on Sep 5, 2021
 History
 2 contributors
@mfisher31@atsushieno
913 lines (767 sloc)  26.2 KB

/*
    Copyright (c) 2014-2019  Michael Fisher <mfisher@kushview.net>
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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#define JLV2_MODULE_EVENT_BUFFER_SIZE   4096
#define JLV2_MODULE_RING_BUFFER_SIZE    4096

namespace jlv2 {

enum UIQuality {
    UI_NO_SUPPORT     = 0,    // UI not supported
    UI_FULL_SUPPORT   = 1,    // UI directly embeddable (i.e. a juce component)
    UI_NATIVE_EMBED   = 2     // Embeddable Native UI type
};

struct SortSupportedUIs
{
    // - a value of < 0 if the first comes before the second
    // - a value of 0 if the two objects are equivalent
    // - a value of > 0 if the second comes before the first
    int compareElements (const SupportedUI* first, const SupportedUI* second)
    {
        // noop
        return 0;
    }
};

class Module::Private
{
public:
    Private (Module& module)
        : owner (module) { }

    ~Private() { }

    InstanceUI* createInstanceUI (const SupportedUI& supportedUI)
    {
        auto* uiptr = new InstanceUI (owner.getWorld(), owner);
        uiptr->ui               = supportedUI.URI;
        uiptr->plugin           = supportedUI.plugin;
        uiptr->container_type    = supportedUI.container;
        uiptr->widget_type       = supportedUI.widget;
        uiptr->bundle_path       = supportedUI.bundle;
        uiptr->binary_path       = supportedUI.binary;
        uiptr->require_show      = supportedUI.useShowInterface;
        this->ui = uiptr;
        return uiptr;
    }

    void sendControlValues()
    {
        if (! ui && ! owner.onPortNotify)
            return;

        for (const auto* port : ports.getPorts())
        {
            if (PortType::Control != port->type)
                continue;

            auto* const buffer = buffers.getUnchecked (port->index);

            if (ui)
                ui->portEvent ((uint32_t)port->index, sizeof(float), 
                               0, buffer->getPortData());
            if (owner.onPortNotify)
                owner.onPortNotify ((uint32_t) port->index, sizeof(float), 
                                    0, buffer->getPortData());
        }
    }

    static const void * getPortValue (const char *port_symbol, void *user_data, uint32_t *size, uint32_t *type)
    {
        Module::Private* priv = static_cast<Module::Private*> (user_data);
        int portIdx = -1;
        for (const auto* port : priv->ports.getPorts())
        {
            if (port->symbol == port_symbol && port->type == PortType::Control) {
                portIdx = port->index;
                break;
            }
        }

        if (portIdx >= 0)
        {
            if (auto* const buffer = priv->buffers [portIdx])
            {
                *size = sizeof (float);
                *type = priv->owner.map (LV2_ATOM__Float);
                return buffer->getPortData();
            }
        }

        *size = 0;
        *type = 0;

        return nullptr;
    }

    static void setPortValue (const char* port_symbol,
                              void*       user_data,
                              const void* value,
                              uint32_t    size,
                              uint32_t    type)
    {
        auto* priv = (Private*) user_data;
        auto& plugin = priv->owner;

        if (type != priv->owner.map (LV2_ATOM__Float))
            return;

        int portIdx = -1;
        const PortDescription* port = nullptr;
        for (const auto* p : priv->ports.getPorts())
        {
            port = p;
            if (port->symbol == port_symbol && port->type == PortType::Control) {
                portIdx = port->index;
                break;
            }
        }

        if (portIdx >= 0 && port)
        {
            if (auto* const buffer = priv->buffers [portIdx])
                buffer->setValue (*((float*) value));
        }
    }
    
private:
    friend class Module;
    Module& owner;
    PortList ports;
    ChannelConfig channels;

    String uri;     ///< plugin URI
    String name;    ///< Plugin name
    String author;  ///< Plugin author name

    InstanceUI::Ptr ui;

    HeapBlock<float> mins, maxes, defaults;    
    OwnedArray<PortBuffer> buffers;

    LV2_Feature instanceFeature { LV2_INSTANCE_ACCESS_URI, nullptr };
};

Module::Module (World& world_, const void* plugin_)
   : instance (nullptr),
     plugin ((const LilvPlugin*) plugin_),
     world (world_),
     active (false),
     currentSampleRate (44100.0),
     numPorts (lilv_plugin_get_num_ports (plugin)),
     events (nullptr)
{
    priv = new Private (*this);
    init();
}

Module::~Module()
{
    freeInstance();
    worker = nullptr;
}

void Module::activatePorts()
{
    // noop
}

void Module::init()
{
    events.reset (new RingBuffer (JLV2_MODULE_RING_BUFFER_SIZE));
    evbufsize = jmax (evbufsize, static_cast<uint32> (JLV2_MODULE_RING_BUFFER_SIZE));
    evbuf.realloc (evbufsize);
    evbuf.clear (evbufsize);

    notifications.reset (new RingBuffer (JLV2_MODULE_RING_BUFFER_SIZE));
    ntbufsize = jmax (ntbufsize, static_cast<uint32> (JLV2_MODULE_RING_BUFFER_SIZE));
    ntbuf.realloc (ntbufsize);
    ntbuf.clear (ntbufsize);

    // create and set default port values
    priv->mins.allocate (numPorts, true);
    priv->maxes.allocate (numPorts, true);
    priv->defaults.allocate (numPorts, true);

    lilv_plugin_get_port_ranges_float (plugin, priv->mins, priv->maxes, priv->defaults);

    // initialize each port
    for (uint32 p = 0; p < numPorts; ++p)
    {
        const LilvPort* port (lilv_plugin_get_port_by_index (plugin, p));

        // port type
        PortType type = PortType::Unknown;
        if (lilv_port_is_a (plugin, port, world.lv2_AudioPort))
            type = PortType::Audio;
        else if (lilv_port_is_a (plugin, port, world.lv2_AtomPort))
            type = PortType::Atom;
        else if (lilv_port_is_a (plugin, port, world.lv2_ControlPort))
            type = PortType::Control;
        else if (lilv_port_is_a (plugin, port, world.lv2_CVPort))
            type = PortType::CV;
        else if (lilv_port_is_a (plugin, port, world.lv2_EventPort))
            type = PortType::Event;
            
        const bool isInput (lilv_port_is_a (plugin, port, world.lv2_InputPort));
        LilvNode* nameNode = lilv_port_get_name (plugin, port);
        const String name = lilv_node_as_string (nameNode);
        lilv_node_free (nameNode); nameNode = nullptr;
        const String symbol  = lilv_node_as_string (lilv_port_get_symbol (plugin, port));

        priv->ports.add (type, p, priv->ports.size (type, isInput),
                         symbol, name, isInput);
        priv->channels.addPort (type, p, isInput);

        uint32 capacity = sizeof (float);
        uint32 dataType = 0;
        switch ((uint32) type.id()) 
        {
            case PortType::Control:
                capacity = sizeof (float); 
                dataType = map (LV2_ATOM__Float);
                break;
            case PortType::Audio:
                capacity = sizeof (float);
                dataType = map (LV2_ATOM__Float);
                break;
            case PortType::Atom:
                capacity = JLV2_MODULE_EVENT_BUFFER_SIZE;
                dataType = map (LV2_ATOM__Sequence);
                break;
            case PortType::Midi:    
                capacity = sizeof (uint32); 
                dataType = map (LV2_MIDI__MidiEvent);
                break;
            case PortType::Event:
                capacity = JLV2_MODULE_EVENT_BUFFER_SIZE; 
                dataType = map (LV2_EVENT__Event);
                break;
            case PortType::CV:      
                capacity = sizeof(float);
                dataType = map (LV2_ATOM__Float);
                break;
        }

        PortBuffer* const buf = priv->buffers.add (
            new PortBuffer (isInput, type, dataType, capacity));
        
        if (type == PortType::Control)
            buf->setValue (priv->defaults [p]);
    }

    // load related GUIs
    if (auto* related = lilv_plugin_get_related (plugin, world.ui_UI))
    {
        LILV_FOREACH (nodes, iter, related)
        {
            const auto* res = lilv_nodes_get (related, iter);
            lilv_world_load_resource (world.getWorld(), res);
        }
        lilv_nodes_free (related);
    }

    // plugin URI
    priv->uri = String::fromUTF8 (lilv_node_as_string (lilv_plugin_get_uri (plugin)));

    // plugin name
    if (LilvNode* node = lilv_plugin_get_name (plugin))
    {
        priv->name = String::fromUTF8 (lilv_node_as_string (node));
        lilv_node_free (node);
    }

    // author name
    if (LilvNode* node = lilv_plugin_get_author_name (plugin))
    {
        priv->author = String::fromUTF8 (lilv_node_as_string (node));
        lilv_node_free (node);
    }
}

void Module::loadDefaultState()
{
    if (instance == nullptr)
        return;
    
    auto* const map = (LV2_URID_Map*) world.getFeatures().getFeature (LV2_URID__map)->getFeature()->data;
    if (auto* uriNode = lilv_new_uri (world.getWorld(), priv->uri.c_str()))
    {
        if (auto* state = lilv_state_new_from_world (world.getWorld(), map, uriNode))
        {
            const LV2_Feature* const features[] = { nullptr };
            lilv_state_restore (state, instance, Private::setPortValue, 
                                priv.get(), LV2_STATE_IS_POD, features);
            lilv_state_free (state);
            priv->sendControlValues();
        }

        lilv_node_free (uriNode);
    }
}

String Module::getStateString() const
{
    if (instance == nullptr)
        return String();
    
    auto* const map = (LV2_URID_Map*) world.getFeatures().getFeature (LV2_URID__map)->getFeature()->data;
    auto* const unmap = (LV2_URID_Unmap*) world.getFeatures().getFeature (LV2_URID__unmap)->getFeature()->data;
    const String descURI = "http://kushview.net/kv/state";

    String result;
    const LV2_Feature* const features[] = { nullptr };
    
    if (auto* state = lilv_state_new_from_instance (plugin, instance, 
        map, 0, 0, 0, 0, 
        Private::getPortValue, priv.get(), 
        LV2_STATE_IS_POD, // flags
        features))
    {
        char* strState = lilv_state_to_string (world.getWorld(), map, unmap, state, descURI.c_str(), 0);
        result = String::fromUTF8 (strState);
        std::free (strState);

        lilv_state_free (state);
    }

    return result;
}

void Module::setStateString (const String& stateStr)
{
    if (instance == nullptr)
        return;
    auto* const map = (LV2_URID_Map*) world.getFeatures().getFeature (LV2_URID__map)->getFeature()->data;
    auto* const unmap = (LV2_URID_Unmap*) world.getFeatures().getFeature (LV2_URID__unmap)->getFeature()->data;
    if (auto* state = lilv_state_new_from_string (world.getWorld(), map, stateStr.c_str()))
    {
        const LV2_Feature* const features[] = { nullptr };
        lilv_state_restore (state, instance, Private::setPortValue, 
                            priv.get(), LV2_STATE_IS_POD, features);
        lilv_state_free (state);
        priv->sendControlValues();
    }
}

Result Module::instantiate (double samplerate)
{
    freeInstance();
    jassert(instance == nullptr);
    currentSampleRate = samplerate;

    features.clearQuick();
    world.getFeatures (features);

    // check for a worker interface
    LilvNodes* nodes = lilv_plugin_get_extension_data (plugin);
    LILV_FOREACH (nodes, iter, nodes)
    {
        const LilvNode* node = lilv_nodes_get (nodes, iter);
        if (lilv_node_equals (node, world.work_interface))
        {
            worker = new WorkerFeature (world.getWorkThread(), 1);
            features.add (worker->getFeature());
        }
    }
    lilv_nodes_free (nodes); nodes = nullptr;
    
    features.add (nullptr);
    instance = lilv_plugin_instantiate (plugin, samplerate,
                                        features.getRawDataPointer());
    if (instance == nullptr) {
        features.clearQuick();
        worker = nullptr;
        return Result::fail ("Could not instantiate plugin.");
    }

    if (const void* data = getExtensionData (LV2_WORKER__interface))
    {
        if (worker == nullptr)
            return Result::fail ("Could not get worker feature whereas extension data exists.");
        worker->setSize (2048);
        worker->setInterface (lilv_instance_get_handle (instance),
                              (LV2_Worker_Interface*) data);
    }
    else if (worker)
    {
        features.removeFirstMatchingValue (worker->getFeature());
        worker = nullptr;
    }

    loadDefaultState();
    startTimerHz (60);
    return Result::ok();
}

void Module::activate()
{
   if (instance && ! active)
   {
       lilv_instance_activate (instance);
       activatePorts();
       active = true;
   }
}

void Module::cleanup()
{
   if (instance != nullptr)
   {
       // TODO: Check if Lilv takes care of calling cleanup
   }
}

void Module::deactivate()
{
   if (instance != nullptr && active)
   {
       lilv_instance_deactivate (instance);
       active = false;
   }
}

bool Module::isActive() const
{
   return instance && active;
}

void Module::freeInstance()
{
    stopTimer();
    if (instance != nullptr)
    {
        deactivate();
        worker = nullptr;
        auto* oldInstance = instance;
        instance = nullptr;
        lilv_instance_free (oldInstance);
    }
}

void Module::setSampleRate (double newSampleRate)
{
    if (newSampleRate == currentSampleRate)
        return;

    if (instance != nullptr)
    {
        const bool wasActive = isActive();
        freeInstance();
        instantiate (newSampleRate);

        jassert (currentSampleRate == newSampleRate);

        if (wasActive)
            activate();
    }
}

void Module::connectChannel (const PortType type, const int32 channel, void* data, const bool isInput)
{
    connectPort (priv->channels.getPort (type, channel, isInput), data);
}

void Module::connectPort (uint32 port, void* data)
{
    lilv_instance_connect_port (instance, port, data);
}

String Module::getURI()         const { return priv->uri; }
String Module::getName()        const { return priv->name; }
String Module::getAuthorName()  const { return priv->author; }

const ChannelConfig& Module::getChannelConfig() const
{
    return priv->channels;
}

String Module::getClassLabel() const
{
   if (const LilvPluginClass* klass = lilv_plugin_get_class (plugin))
       if (const LilvNode* node = lilv_plugin_class_get_label (klass))
           return CharPointer_UTF8 (lilv_node_as_string (node));
   return String();
}

const void* Module::getExtensionData (const String& uri) const
{
    return instance ? lilv_instance_get_extension_data (instance, uri.toUTF8())
                    : nullptr;
}

void* Module::getHandle()
{
    return instance ? (void*) lilv_instance_get_handle (instance)
                    : nullptr;
}

uint32 Module::getNumPorts() const { return numPorts; }

uint32 Module::getNumPorts (PortType type, bool isInput) const
{
    return static_cast<uint32> (priv->ports.size (type, isInput));
}

const LilvPort* Module::getPort (uint32 port) const
{
    return lilv_plugin_get_port_by_index (plugin, port);
}

uint32 Module::getMidiPort() const
{
   for (uint32 i = 0; i < getNumPorts(); ++i)
   {
       const LilvPort* port (getPort (i));
       if ((lilv_port_is_a (plugin, port, world.lv2_AtomPort) || lilv_port_is_a (plugin, port, world.lv2_EventPort)) &&
           lilv_port_is_a (plugin, port, world.lv2_InputPort) &&
           lilv_port_supports_event (plugin, port, world.midi_MidiEvent))
           return i;
   }

   return LV2UI_INVALID_PORT_INDEX;
}

uint32 Module::getNotifyPort() const
{
    for (uint32 i = 0; i < numPorts; ++i)
    {
        const LilvPort* port (getPort (i));
        if (lilv_port_is_a (plugin, port, world.lv2_AtomPort) &&
            lilv_port_is_a (plugin, port, world.lv2_OutputPort) &&
            lilv_port_supports_event (plugin, port, world.midi_MidiEvent))
        {
            return i;
        }
    }

    return LV2UI_INVALID_PORT_INDEX;
}

const LilvPlugin* Module::getPlugin() const { return plugin; }

const String Module::getPortName (uint32 index) const
{
    if (const auto* desc = priv->ports.get (index))
        return desc->name;
    return String();
}

void Module::getPortRange (uint32 port, float& min, float& max, float& def) const
{
    if (port >= numPorts)
        return;

    min = priv->mins [port];
    max = priv->maxes [port];
    def = priv->defaults [port];
}

PortType Module::getPortType (uint32 index) const
{
    if (const auto* desc = priv->ports.get (index))
        return desc->type >= PortType::Control && desc->type <= PortType::Unknown 
            ? desc->type : PortType::Unknown;
    return PortType::Unknown;
}

ScalePoints Module::getScalePoints (uint32 index) const
{
    ScalePoints sps;

    if (const auto* port = lilv_plugin_get_port_by_index (plugin, index))
    {
        if (auto* points = lilv_port_get_scale_points (plugin, port))
        {
            LILV_FOREACH(scale_points, iter, points)
            {
                const auto* point = lilv_scale_points_get (points, iter);
                sps.points.set (
                    String::fromUTF8 (lilv_node_as_string (lilv_scale_point_get_label (point))),
                    lilv_node_as_float (lilv_scale_point_get_value (point))
                );
            }

            lilv_scale_points_free (points);
        }
    }

    return sps;
}

bool Module::isPortEnumerated (uint32 index) const
{
    if (const auto* const port = lilv_plugin_get_port_by_index (plugin, index))
        return lilv_port_has_property (plugin, port, world.lv2_enumeration);
    return false;
}

bool Module::isLoaded() const { return instance != nullptr; }

static SupportedUI* createSupportedUI (const LilvPlugin* plugin, const LilvUI* ui)
{
    auto entry = new SupportedUI();
    entry->URI      = lilv_node_as_uri (lilv_ui_get_uri (ui));
    entry->plugin   = lilv_node_as_uri (lilv_plugin_get_uri (plugin));
    entry->bundle   = lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_bundle_uri (ui)));
    entry->binary   = lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_binary_uri (ui)));
    entry->useShowInterface = false;
    return entry;
}

bool Module::hasEditor() const
{
    if (! supportedUIs.isEmpty())
        return true;

    LilvUIs* uis = lilv_plugin_get_uis (plugin);
    if (nullptr == uis)
        return false;

    auto& suplist = const_cast<OwnedArray<SupportedUI>&> (supportedUIs);
    
    LILV_FOREACH (uis, iter, uis)
    {
        const LilvUI* lui = lilv_uis_get (uis, iter);
        bool hasShow = false;
        bool hasIdle = false;

        // check for extension data
        {
            auto* uriNode = lilv_new_uri (world.getWorld(), lilv_node_as_uri (lilv_ui_get_uri (lui)));
            auto* extDataNode = lilv_new_uri (world.getWorld(), LV2_CORE__extensionData);
            auto* showNode = lilv_new_uri (world.getWorld(), LV2_UI__showInterface);
            auto* idleNode = lilv_new_uri (world.getWorld(), LV2_UI__idleInterface);
            
            if (auto* extNodes = lilv_world_find_nodes (world.getWorld(), uriNode, extDataNode, nullptr))
            {
                LILV_FOREACH(nodes, iter, extNodes)
                {
                    const auto* node = lilv_nodes_get (extNodes, iter);
                    if (lilv_node_equals (node, showNode))
                        hasShow = true;
                    else if (lilv_node_equals (node, idleNode))
                        hasIdle = true;
                }

                lilv_nodes_free (extNodes);
            }
            
            lilv_node_free (uriNode);
            lilv_node_free (extDataNode);
            lilv_node_free (showNode);
            lilv_node_free (idleNode);
        }

        // Check JUCE UI
        if (lilv_ui_is_a (lui, world.ui_JUCEUI))
        {
            auto* const s = suplist.add (createSupportedUI (plugin, lui));
            s->container = JLV2__JUCEUI;
            s->widget = JLV2__JUCEUI;
            continue;
        }

        const LilvNode* uitype = nullptr;

        // check if native UI
        if (lilv_ui_is_supported (lui, suil_ui_supported, world.getNativeWidgetType(), &uitype))
        {
            if (uitype != nullptr && lilv_node_is_uri (uitype))
            {
                auto* const s = suplist.add (createSupportedUI (plugin, lui));
                s->container = LVTK_UI__NativeUI;
                s->widget    = String::fromUTF8 (lilv_node_as_uri (uitype));
                continue;
            }
        }

        // no UI this far, check show interface
        if (hasShow)
        {
            auto* const s = suplist.add (createSupportedUI (plugin, lui));
            s->useShowInterface = true;
            s->container = LV2_UI__showInterface;
            s->widget    = LV2_UI__showInterface;
        }
    }

    lilv_uis_free (uis);

    SortSupportedUIs sorter;
    suplist.sort (sorter, true);

    for (const auto* const sui : supportedUIs)
    {
        DBG("[jlv2] supported ui: " << sui->URI);
        DBG("[jlv2]    container: " << sui->container);
        DBG("[jlv2]       widget: " << sui->widget);
        DBG("[jlv2]         show: " << (int) sui->useShowInterface);
    }

    return ! supportedUIs.isEmpty();
}

void Module::clearEditor()
{
    if (priv->ui)
    {
        auto ui = priv->ui;
        priv->ui = nullptr;
        ui->unload();
        ui = nullptr;
    }
}

PortBuffer* Module::getPortBuffer (uint32 port) const
{
    jassert (port < numPorts);
    return priv->buffers.getUnchecked (port);
}

uint32 Module::getPortIndex (const String& symbol) const
{
    for (const auto* port : priv->ports.getPorts())
        if (port->symbol == symbol)
            return static_cast<uint32> (port->index);
    return LV2UI_INVALID_PORT_INDEX;
}

InstanceUI* Module::createEditor()
{
    if (priv->ui)
        return priv->ui.get();
    
    InstanceUI* instance = nullptr;

    for (const auto* const u : supportedUIs)
    {
        if (u->container == LVTK_UI__NativeUI)
            instance = priv->createInstanceUI (*u);
        else if (u->useShowInterface)
            instance = priv->createInstanceUI (*u);
        if (instance != nullptr)
            break;
    }

    if (instance != nullptr)
    {
        jassert (instance == priv->ui.get());
        priv->sendControlValues();
    }

    return instance;
}

void Module::sendPortEvents()
{
    priv->sendControlValues();
}

bool Module::isPortInput (uint32 index) const
{
   return lilv_port_is_a (plugin, getPort (index), world.lv2_InputPort);
}

bool Module::isPortOutput (uint32 index) const
{
   return lilv_port_is_a (plugin, getPort (index), world.lv2_OutputPort);
}

void Module::timerCallback()
{
    PortEvent ev;
    
    static const uint32 pnsize = sizeof (PortEvent);

    for (;;)
    {
        if (! notifications->canRead (pnsize))
            break;

        notifications->read (ev, false);
        if (ev.size > 0 && notifications->canRead (pnsize + ev.size))
        {
            notifications->advance (pnsize, false);
            notifications->read (ntbuf, ev.size, true);

            if (ev.protocol == 0)
            {
                if (auto ui = priv->ui)
                    ui->portEvent (ev.index, ev.size, ev.protocol, ntbuf.getData());
                if (onPortNotify)
                    onPortNotify (ev.index, ev.size, ev.protocol, ntbuf.getData());
            }
        }
    }
}

void Module::referAudioReplacing (AudioSampleBuffer& buffer)
{
    for (int c = 0; c < priv->channels.getNumAudioInputs(); ++c)
        priv->buffers.getUnchecked ((int) priv->channels.getPort (
            PortType::Audio, c, true))->referTo (buffer.getWritePointer (c));

    for (int c = 0; c < priv->channels.getNumAudioOutputs(); ++c)
        priv->buffers.getUnchecked ((int) priv->channels.getPort (
            PortType::Audio, c, false))->referTo (buffer.getWritePointer (c));
}

void Module::run (uint32 nframes)
{
    PortEvent ev;
    
    static const uint32 pesize = sizeof (PortEvent);

    for (;;)
    {
        if (! events->canRead (pesize))
            break;
        events->read (ev, false);
        if (ev.size > 0 && events->canRead (pesize + ev.size))
        {
            events->advance (pesize, false);
            events->read (evbuf, ev.size, true);

            if (ev.protocol == 0)
            {
                auto* buffer = priv->buffers.getUnchecked (ev.index);                
                if (buffer->getValue() != *((float*) evbuf.getData()))
                {
                    buffer->setValue (*((float*) evbuf.getData()));
                    if (notifications->canWrite (pesize + ev.size)) 
                    {
                        notifications->write (ev);
                        notifications->write (evbuf.getData(), ev.size);
                    }
                }
            }
        }
    }

    for (int i = priv->buffers.size(); --i >= 0;)
        connectPort (static_cast<uint32> (i), priv->buffers.getUnchecked(i)->getPortData());
    
    if (worker)
        worker->processWorkResponses();

    lilv_instance_run (instance, nframes);

    if (worker)
        worker->endRun();
}

uint32 Module::map (const String& uri) const
{
    // FIXME: const in SymbolMap::map/unmap 
    return (const_cast<World*> (&world))->map (uri);
}

void Module::write (uint32 port, uint32 size, uint32 protocol, const void* buffer)
{
    PortEvent event;
    zerostruct (event);
    event.index     = port;
    event.size      = size;
    event.protocol  = protocol;

    if (events->canWrite (sizeof (PortEvent) + size))
    {
        events->write (event);
        events->write (buffer, event.size);
    }
    else
    {
        DBG("lv2 plugin write buffer full.");
    }
}

}
Footer
© 2022 GitHub, Inc.
Footer navigation
Terms
Privacy
Security
Status
Docs
Contact GitHub
Pricing
API
Training
Blog
About
#endif
