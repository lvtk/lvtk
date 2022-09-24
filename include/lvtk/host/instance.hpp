// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <lvtk/lvtk.hpp>

namespace lvtk {
namespace detail {
/** @private */
class World;
/** @private */
class Instance;
/** @private */
class InstanceUI;
} // namespace detail

class World;
class InstanceUI;

/** A UI that is supported for loading */
struct LVTK_API SupportedUI {
    std::string URI;        ///< UI's URI
    std::string container;  ///< Container type URI
    std::string widget;     ///< Widget Type URI
    bool show { false };    ///< Wants show interface
    bool idle { false };    ///< Wants idle interface
    unsigned quality { 0 }; ///< Quality of the GUI
};

/** A vector of supported UIs */
class LVTK_API SupportedUIs : public std::vector<SupportedUI> {};

/** Details about an LV2 plugin */
struct LVTK_API PluginInfo {
    std::string URI;             ///< Plugin's URI
    std::string name;            ///< Plugin Name
    std::string author_name;     ///< Plugin author
    std::string author_homepage; ///< Plugin homepage URL
    std::string author_email;    ///< Author's email
    SupportedUIs uis;            ///< UIs supported by the plugin.
};

/** A LV2 plugin instance.
 
    Powerd by Lilv
*/
class LVTK_API Instance final {
public:
    ~Instance();

    /** Returns this Plugin's name */
    std::string name() const noexcept;

    Handle handle() const noexcept;

    void activate();
    void connect_port (uint32_t port, void* data);
    void run (uint32_t nframes);
    void deactivate();

    const PluginInfo& info() const noexcept;

    std::unique_ptr<InstanceUI> instantiate_ui (uintptr_t parent) const noexcept;

private:
    friend class detail::Instance;
    friend class World;
    friend class detail::World;
    friend class InstanceUI;
    friend class detail::InstanceUI;

    std::unique_ptr<detail::Instance> impl;
    Instance (World&);

    uint32_t port_index (const char*) { return 0; }
    void write (uint32_t port, uint32_t size, uint32_t protocol, const void* data);
};

/** An Instance's UI */
class LVTK_API InstanceUI final {
public:
    ~InstanceUI();

    /** Set this to handle touch notifications from the UI */
    std::function<void (uint32_t, bool)> touched;

    /** Called when the UI invokes LV2UI_Resize::ui_resize 
        Use requestSize to request the UI changes it size from the host
        return zero on success
    */
    std::function<int()> client_resized;

    /** Returns true if the UI has been instantiated */
    bool loaded() const;

    /** Unload if loaded */
    void unload();

    /** Get the world for this instance */
    World& world() const;

    /** Get the plugin instance this GUI is controlling */
    Instance& plugin_instance() const;

    /** Get the widget provided by the UI */
    LV2UI_Widget widget() const;

    void port_event (uint32_t port, uint32_t size, uint32_t format, const void* buffer);
    bool is_native() const;
    bool has_container_type (const std::string& type) const;
    bool is_a (const std::string& widget_type_uri) const;

    bool have_show_interface() const;
    bool requires_show_interface() const;

    /** Invoke show interface if supported */
    bool show();

    /** Invoke hide interface if supported */
    bool hide();

    /** Returns true if the plugin provided LV2_UI__idleInterface */
    bool have_idle_interface() const;

    /** Idle the GUI if supported */
    void idle();

    /** returs true if the UI provided LV2_UI__resize */
    bool have_client_resize() const;

    /** Request the UI update it's size. This is the Host side of
        LV2_UI__resize
     */
    bool request_size (int width, int height);

    /** Returns the width as reported by UI-side LV2_UI__resize */
    int width() const noexcept;

    /** Returns the width as reported by UI-side LV2_UI__resize */
    int height() const noexcept;

private:
    friend class Instance;
    friend class detail::Instance;
    friend class detail::InstanceUI;
    friend class World;
    friend class detail::World;
    InstanceUI() = delete;
    InstanceUI (World& w, Instance& m);
    std::unique_ptr<detail::InstanceUI> impl;
};

} // namespace lvtk
