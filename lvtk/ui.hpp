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

#pragma once

#include <lvtk/lvtk.hpp>
#include <lvtk/feature.hpp>
#include <lv2/ui/ui.h>

namespace lvtk {
namespace ui {

using Descriptors = DescriptorList<LV2UI_Descriptor>;

static Descriptors& descriptors() {
    static Descriptors s_desc;
    return s_desc;
}

class Controller {
public:
    Controller (LV2UI_Controller c, LV2UI_Write_Function f)
        : controller (c), port_write (f) { }
    Controller (const Controller& o) { operator= (o); }

    void write (uint32_t port, uint32_t size, uint32_t protocol, const void * data) const {
        port_write (controller, port, size, protocol, data);
    }

    LV2UI_Controller c_obj() const { return controller; }
    operator LV2UI_Controller() const { return controller; }

    Controller& operator= (const Controller& o) {
        controller = o.controller;
        port_write = o.port_write;
        return *this;
    }

private:
    LV2UI_Controller controller = nullptr;
    LV2UI_Write_Function port_write = nullptr;
};

class Instance {
public:
    struct Args {
        Args (const String& p, const String& b, const Controller& c, const FeatureList& f)
            : plugin(p), bundle(b), controller(c), features (f)
        String plugin;
        String bundle;
        Controller controller;
        FeatureList features;
    };

    Instance (Args& args) 
        : controller (args.controller)
    {
        for (const auto& f : args.features) {
            if (strcmp (f->URI, LV2_UI__parent)) {
                parent_widget = (LV2_Widget) f->data;
            }
            else if (strcmp (f->URI, LV2_UI__portSubscribe)) {
                subscribe = *(LV2UI_Port_Subscribe*) f->data);
            }
            else if (strcmp (f->URI, LV2_UI__touch)) {
                ui_touch = *(LV2UI_Touch*) f->data;
            }
            else if (strcmp (f->URI, LV2_UI__portMap)) {
                port_map = *(LV2UI_Port_Map*) f->data;
            }
            else if (strcmp (f->URI, LV2_UI__resize)) {
                ui_resize = *(LV2UI_Resize*) f->data;
            }
        }
    }

    virtual ~Instance() = default;

    void cleanup() {}

    LV2UI_Widget get_widget() { return nullptr; }

    void port_event (uint32_t port, uint32_t size, uint32_t format, const void* data) { }

    uint32_t port_index (const String& symbol) const {
        if (port_map.port_index)
            return port_map.port_index (port_map.handle, symbol.c_str())
        return LV2UI_INVALID_PORT_INDEX;
    }

    void port_subscribe (uint32_t port, uint32_t protocol, const FeatureList& features = FeatureList()) {
        if (subscribe.subscribe)
            subscribe.subscribe (subscribe.handle, port, protocol, features)
    }

    void port_unsubscribe (uint32_t port, uint32_t protocol, const FeatureList& features = FeatureList()) {
        if (subscribe.unsubscribe)
            subscribe.unsubscribe (subscribe.handle, port, protocol, features)
    }

    void write (uint32_t port, uint32_t size, uint32_t proto, const void* data) {
        controller.write (port, size, proto, data);
    }

protected:
    const Controller controller;

private:
    LV2UI_Widget parent_widget = nullptr;
    LV2UI_Port_Subscribe subscribe { nullptr, nullptr, nullptr };
    LV2UI_Port_Map port_map { nullptr, nullptr; }
    LV2UI_Touch ui_touch = { 0, 0 }
    LV2UI_Resize ui_resize = { 0, 0 }
};

template<class InstanceType, class...E>
class UI {
public:
    UI (const String& uri, const StringArray& required = { }) 
    {
        LV2UI_Descriptor desc;
        desc.URI = strdup (uri.c_str());
        desc.instantiate = _instantiate;
        desc.port_event = _port_event;
        desc.cleanup = _cleanup;
        desc.extension_data = _extension_data;
        descriptors().push_back (desc);

        for (const auto& rq : required)
            m_required.push_back (rq);
    }

    inline static void register_extension (const std::string& uri, const void* data) {
        s_extensions[uri] = data;
    }

private:
    static ExtensionMap s_extensions;
    static StringArray  s_required;

    static LV2UI_Handle _instantiate (const struct _LV2UI_Descriptor* descriptor,
	                            const char*                     plugin_uri,
	                            const char*                     bundle_path,
	                            LV2UI_Write_Function            write_function,
	                            LV2UI_Controller                ctl,
	                            LV2UI_Widget*                   widget,
	                            const LV2_Feature* const*       feats)
    {
        const FeatureList features (feats);
        const Controller controller (ctl, write_function);

        auto instance = std::unique_ptr<InstanceType> (new InstanceType (
            plugin_uri, bundle_path, controller, features
        ));

        for (const auto& rq : s_required)
        {
            std::clog << "[lvtk] " << rq << ": ";
            bool provided = false;
            for (const auto& f : features)
                if (strcmp (f->URI, rq.c_str()) == 0)
                    { provided = true; break; }
            
            if (! provided) {
                std::clog << "error - not provided by host\n";
                return nullptr;
            }
            else 
            {
                std::clog << "ok";
            }
        }

        *widget = instance->get_widget();
        return static_cast<LV2UI_Handle> (instance.release());
    }

	/**
	   Destroy the UI.  The host must not try to access the widget after
	   calling this function.
	*/
	static void _cleanup (LV2UI_Handle ui)
    {
        (static_cast<InstanceType*>(ui))->cleanup();
        delete static_cast<InstanceType*> (ui);
    }

	static void _port_event (LV2UI_Handle ui,
	                         uint32_t     port_index,
	                         uint32_t     buffer_size,
	                         uint32_t     format,
	                         const void*  buffer)
    {
        (static_cast<InstanceType*>(ui))->port_event(
            port_index, buffer_size, format, buffer);
    }

    static void _extension_data (const char* uri) {
        if (strcmp (uri, LV2_UI__idleInterface)) {
            static LV2UI_Idle_Interface idle = { _idle };
            return &idle;
        }

        if (strcmp (uri, LV2_UI__showInterface)) {
            static LV2UI_Show_Interface show = { _show, _hide };
            return &show;
        }

        auto e = s_extensions.find (uri);
        return e != s_extensions.end() ? e->second : nullptr;
    }

    static uint32_t _show (LV2UI_Handle ui) {
        (static_cast<InstanceType*> (ui))->show();
    }

    static uint32_t _hide (LV2UI_Handle ui) {
        (static_cast<InstanceType*> (ui))->hide();
    }

    static uint32_t _idle (LV2UI_Handle ui) {
        (static_cast<InstanceType*> (ui))->idle();
    }
};

template<class PluginType> ExtensionMap Plugin<PluginType>::s_extensions = {};
template<class PluginType> StringArray Plugin<PluginType>::s_required;

}}

extern "C" {

#ifndef LVTK_NO_SYMBOL_EXPORT

LV2_SYMBOL_EXPORT const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index) {
    return (index < lvtk::ui::descriptors().size())
        ? &lvtk::ui::descriptors()[index] : NULL;
}

#endif

}
