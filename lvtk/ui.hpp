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

#include <memory>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lvtk/lvtk.hpp>

namespace lvtk {
/** @defgroup ui UI 
    Writing an LV2 UI

    UI descriptors are registered on the stack at the global scope.
    For example...
    
    @code
        using MyPluginUI = lvtk::UI<MyInstanceUI>;        
        static MyPluginUI my_plugin (MY_PUGIN_UI_URI);
    @endcode

    If you are using this library in a host, include individual
    headers from the ext directory.

    Same policy for the interface directory, including anything
    there will result in a mess of compiler errors.  Interfaces
    are for implementing features on a plugin instance only

    @see @ref UIInstance
    @{ 
 */

/** Vector of LV2UI_Descriptor's */
using UIDescriptors = DescriptorList<LV2UI_Descriptor>;

/** Returns a global array of registered descriptors */
static UIDescriptors& ui_descriptors() {
    static UIDescriptors s_desc;
    return s_desc;
}

/** A UI Controller
    
    This wraps LV2UI_Controller + LV2UI_Write_Function in a single
    Object

    @headerfile lvtk/ui.hpp
 */
class Controller
{
public:
    Controller() = default;
    Controller (LV2UI_Controller c, LV2UI_Write_Function f)
        : controller (c), port_write (f) { }

    Controller (const Controller& o) { operator= (o); }

    /** Write data to one of the ports */
    void write (uint32_t port, uint32_t size, uint32_t protocol, const void * data) const {
        port_write (controller, port, size, protocol, data);
    }

    /** Get the underlying C type controller */
    LV2UI_Controller c_obj() const { return controller; }

    operator LV2UI_Controller() const { return controller; }

    /** Reference the underlying controller and port write function */
    Controller& operator= (const Controller& o) {
        controller = o.controller;
        port_write = o.port_write;
        return *this;
    }

private:
    LV2UI_Controller controller = nullptr;
    LV2UI_Write_Function port_write = nullptr;
};

/** Parameters passed to UI instances */
struct UIArgs
{
    /** @private */
    UIArgs() = default;
    /** @private */
    UIArgs (const std::string& p, const std::string& b, const Controller& c, const FeatureList& f)
        : plugin(p), bundle(b), controller(c), features (f) {}

    std::string plugin;     /**< Plugin URI */
    std::string bundle;     /**< UI Bundle Path */
    Controller controller;  /**< The @ref Controller */
    FeatureList features;   /**< Feature List */
};

/** UI registration class
    Create a static one of these to register the descriptor for UI instance type.

    @headerfile lvtk/ui.hpp
 */
template<class I>
class UI final
{
public:
    /** UI Registation
        
        @param uri          The URI string of your UI
        @param required     List of required host feature URIs. If the host fails
                            to provide any of these, instantiate will return
                            a nullptr
     */
    UI (const std::string& uri, const std::vector<std::string>& required)
    {
        register_instance (uri.c_str());
        for (const auto& rq : required)
            UI<I>::required().push_back (rq);
    }

    /** UI Registation
        
        @param uri          The URI string of your UI
     */
    UI (const std::string& uri)
    {
        register_instance (uri.c_str());
    }

    /** Register UI extension data but not having to implement
        the a mixin interface.

        @param uri      The uri of your feature.
        @param data     Pointer to static extension data.
     */
    inline static void register_extension (const std::string& uri, const void* data) {
        extensions()[uri] = data;
    }

private:
    void register_instance (const char* uri) {
        LV2UI_Descriptor desc;
        desc.URI = strdup (uri);
        desc.instantiate = _instantiate;
        desc.port_event = _port_event;
        desc.cleanup = _cleanup;
        desc.extension_data = _extension_data;
        ui_descriptors().push_back (desc);

        auto& extmap = extensions();
        I::map_extension_data (extmap);
    }

    inline static ExtensionMap& extensions() {
        static ExtensionMap s_extensions;
        return s_extensions;
    }

    inline static std::vector<std::string>& required() {
        static std::vector<std::string>  s_required;
        return s_required;
    }

    static LV2UI_Handle _instantiate (const struct _LV2UI_Descriptor* descriptor,
                                      const char*                     plugin_uri,
                                      const char*                     bundle_path,
                                      LV2UI_Write_Function            write_function,
                                      LV2UI_Controller                ctl,
                                      LV2UI_Widget*                   widget,
                                      const LV2_Feature* const*       features)
    {
        UIArgs args (plugin_uri, bundle_path, { ctl, write_function }, features);
        auto instance = std::unique_ptr<I> (new I (args));

        for (const auto& rq : required())
        {
            bool provided = false;
            for (const auto& f : args.features)
                if (strcmp (f.URI, rq.c_str()) == 0)
                    { provided = true; break; }
            
            if (! provided) {
                return nullptr;
            }
            else 
            {
            }
        }

        *widget = instance->get_widget();
        return static_cast<LV2UI_Handle> (instance.release());
    }

	static void _cleanup (LV2UI_Handle ui)
    {
        (static_cast<I*>(ui))->cleanup();
        delete static_cast<I*> (ui);
    }

	static void _port_event (LV2UI_Handle ui,
	                         uint32_t     port_index,
	                         uint32_t     buffer_size,
	                         uint32_t     format,
	                         const void*  buffer)
    {
        (static_cast<I*>(ui))->port_event (port_index, buffer_size, format, buffer);
    }

    static const void* _extension_data (const char* uri) {
        auto e = extensions().find (uri);
        return e != extensions().end() ? e->second : nullptr;
    }
};

/** UI Instance 
 
    Inherrit this when making an LV2 UI.  The first template parameter
    is the type of your super class.  The rest are feature mixins.

    @headerfile lvtk/ui.hpp
*/
template<class S, template<class> class... E>
class UIInstance : public E<S>...
{
public:
    explicit UIInstance (const UIArgs& args) : E<S> (args.features)..., controller (args.controller)
    {
        for (const auto& f : args.features) {
            if (f == LV2_UI__parent) {
                parent_widget = (LV2UI_Widget) f.data;
            } else if (f == LV2_UI__portSubscribe) {
                subscribe = *(LV2UI_Port_Subscribe*) f.data;
            } else if (f == LV2_UI__touch) {
                ui_touch = *(LV2UI_Touch*) f.data;
            } else if (f == LV2_UI__portMap) {
                port_map = *(LV2UI_Port_Map*) f.data;
            } else if (f == LV2_UI__resize) {
                ui_resize = *(LV2UI_Resize*) f.data;
            }
        }
    }

    virtual ~UIInstance() {}

    /** Clean up (optional)
         
        This is called immediately before the dtor 
     */
    void cleanup() { }

    /** Get the LV2UI_Widget (required)
        
        It is ok to create your widget here, but make sure
        that it only gets allocated once. 
    */
    LV2UI_Widget get_widget() { return nullptr; }

    /** Port events (optional) 
     
        Called when port events are received from the host. Implement this to 
        update the UI when properties change in the plugin.
    */
    void port_event (uint32_t port, uint32_t size, uint32_t format, const void* data) { }

    /** Write data to ports
        
        @param port
        @param size
        @param protocol
        @param data
     */
    inline void write (uint32_t port, uint32_t size, uint32_t protocol, const void* data) const {
        controller.write (port, size, protocol, data);
    }

    /** Write a float to a control port */
    inline void write (uint32_t port, float value) const {
        write (port, sizeof (float), 0, &value);
    }

    /** Port index map (optional) 

        Only returns valid port indexes if the host provides LV2UI_Port_Map
        during instantiation.

        @param symbol   The symbol to return an index for
    */
    uint32_t port_index (const std::string& symbol) const {
        if (port_map.port_index)
            return port_map.port_index (port_map.handle, symbol.c_str());
        return LV2UI_INVALID_PORT_INDEX;
    }

    /** Subscribe to port notifications (optional)
        
        This method works only if the host provides LV2UI_Port_Subscribe
        during instantiation

        @param port
        @param protocol
        @param features
     */
    void port_subscribe (uint32_t port, uint32_t protocol, const FeatureList& features = FeatureList()) {
        LV2_Feature* f [features.size() + 1];
        for (int i = 0; i < features.size(); ++i)
            f[i] = (LV2_Feature*)(&features[i]);
        f[features.size()] = nullptr;

        if (subscribe.subscribe)
            subscribe.subscribe (subscribe.handle, port, protocol, f);
    }

    /** Unsubscribe from port notifications (optional)
        
        This method works only if the host provides LV2UI_Port_Subscribe
        during instantiation

        @param port
        @param protocol
        @param features
     */
    void port_unsubscribe (uint32_t port, uint32_t protocol, const FeatureList& features = FeatureList()) {
        LV2_Feature* f [features.size() + 1];
        for (int i = 0; i < features.size(); ++i)
            f[i] = (LV2_Feature*)(&features[i]);
        f[features.size()] = nullptr;
    
        if (subscribe.unsubscribe)
            subscribe.unsubscribe (subscribe.handle, port, protocol, f);
    }

protected:
    const Controller controller;

private:
    LV2UI_Widget parent_widget = nullptr;
    LV2UI_Port_Subscribe subscribe { nullptr, nullptr, nullptr };
    LV2UI_Port_Map port_map { nullptr, nullptr };
    LV2UI_Touch ui_touch = { 0, 0 };
    LV2UI_Resize ui_resize = { 0, 0 };

    friend class UI<S>; // so this can be private
    /** @private */
    static void map_extension_data (ExtensionMap& em) {
        using pack_context = std::vector<int>;
        pack_context { (E<S>::map_extension_data (em) , 0)... };
    }
};

/* @} */
}

extern "C" {

#ifndef LVTK_NO_SYMBOL_EXPORT

LV2_SYMBOL_EXPORT const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index) {
    return (index < lvtk::ui_descriptors().size())
        ? &lvtk::ui_descriptors()[index] : NULL;
}

#endif

}
