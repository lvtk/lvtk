// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <algorithm>
#include <iostream>
#include <vector>

#include <sstream>

#if _WIN32
#    define redirect_pipe 0
#else
#    include <unistd.h>
#    define redirect_pipe 1
#endif

#include <lv2/ui/ui.h>

#include <lvtk/lvtk.hpp>
#include <lvtk/options.hpp>
#include <lvtk/symbols.hpp>

#include <lvtk/host/instance.hpp>
#include <lvtk/host/world.hpp>

#include "instance_impl.hpp"
#include "node.hpp"

namespace lvtk {

namespace detail {
static unsigned ui_supported (const char* host_type, const char* ui_type) {
    if (strcmp (host_type, LVTK_UI__NativeUI) == 0)
        if (strcmp (ui_type, LVTK_UI__NativeUI) == 0)
            return 2;
    return suil_ui_supported (host_type, ui_type);
}

static SupportedUIs supported_uis (LilvWorld* world, const LilvPlugin* plugin) {
    SupportedUIs supported;

    lilv::UIs uis (lilv_plugin_get_uis (plugin));
    if (! uis)
        return supported;

    LILV_FOREACH (uis, iter, uis.get()) {
        const LilvUI* lui = lilv_uis_get (uis, iter);
        auto uri_node     = lilv::Node (lilv_new_uri (world, lilv_node_as_uri (lilv_ui_get_uri (lui))));

        SupportedUI sui;
        sui.show    = false;
        sui.idle    = false;
        sui.quality = 0;

        // check for extension data
        {
            auto ext_data_node = lilv::Node (lilv_new_uri (world, LV2_CORE__extensionData));
            auto show_node     = lilv::Node (lilv_new_uri (world, LV2_UI__showInterface));
            auto idle_node     = lilv::Node (lilv_new_uri (world, LV2_UI__idleInterface));

            if (auto ext_nodes = lilv_world_find_nodes (world, uri_node, ext_data_node, nullptr)) {
                // std::clog << "[world] checking ui extension data:" << std::endl;
                LILV_FOREACH (nodes, iter, ext_nodes) {
                    lilv::Node node (lilv_nodes_get (ext_nodes, iter));
                    // std::clog << "[world]  " << node.as_string() << std::endl;
                    if (lilv_node_equals (node, show_node))
                        sui.show = true;
                    else if (lilv_node_equals (node, idle_node))
                        sui.idle = true;
                }

                lilv_nodes_free (ext_nodes);
            }
        }

        const LilvNode* uitype = nullptr;
        auto nwt               = lilv::Node (lilv_new_uri (world, LVTK_UI__NativeUI));

        // check if native UI
        if (lilv_ui_is_supported (lui, ui_supported, nwt, &uitype)) {
            if (uitype != nullptr && lilv_node_is_uri (uitype)) {
                sui.URI = lilv::Node (lilv_ui_get_uri (lui)).as_string();
                if (! sui.URI.empty()) {
                    sui.container = LVTK_UI__NativeUI;
                    sui.widget    = lilv_node_as_uri (uitype);
                    sui.quality   = ui_supported (LVTK_UI__NativeUI, sui.widget.c_str());
                    supported.push_back (sui);
                    continue;
                }
            }
        }

        // no UI this far, check show interface
        if (sui.show) {
            // std::clog << "[world] supported ui: has show.";
            sui.show      = true;
            sui.container = LV2_UI__showInterface;
            sui.widget    = LV2_UI__showInterface;
            supported.push_back (sui);
            continue;
        }
    }

    auto sorter = [] (const SupportedUI& lhs, const SupportedUI& rhs) { return lhs.quality > rhs.quality; };
    std::sort (supported.begin(), supported.end(), sorter);

#if 0
    for (const auto& s : supported) {
        std::clog << "[lvtk] supported ui: " << s.URI << std::endl;
        std::clog << "[lvtk]    container: " << s.container << std::endl;
        std::clog << "[lvtk]       widget: " << s.widget << std::endl;
        std::clog << "[lvtk]         idle: " << (int) s.idle << std::endl;
        std::clog << "[lvtk]         show: " << (int) s.show << std::endl;
        std::clog << "[lvtk]      quality: " << (int) s.quality << std::endl;
    }
#endif

    return supported;
}

class World {
public:
    using Public = lvtk::World;

    World (Public& o) : owner (o) {
        world                = lilv_world_new();
        options_feature.URI  = LV2_OPTIONS__options;
        options_feature.data = (void*) options.get();
    }

    ~World() {
        if (world != nullptr)
            lilv_world_free (world);
        world = nullptr;
    }

    void load_all() {
#if redirect_pipe
        static constexpr size_t bufsize = 2048 + 1;
        char buffer[bufsize]            = { 0 };
        int out_pipe[2];
        int saved_stderr;

        auto pipe_open = pipe (out_pipe) == 0;
        if (pipe_open) {
            saved_stderr = dup (STDERR_FILENO);
            dup2 (out_pipe[1], STDERR_FILENO); /* redirect stdout to the pipe */
            close (out_pipe[1]);
        }
#endif
        lilv_world_load_all (world);
        std::fflush (stderr);

#if redirect_pipe
        if (pipe_open) {
            (void) buffer;
            // read(out_pipe[0], buffer, bufsize); /* read from pipe into buffer */
            dup2 (saved_stderr, STDERR_FILENO); /* reconnect stdout for testing */
            close (out_pipe[0]);
        }
#endif
#undef bufsize
    }

    const LilvPlugins* plugins() const noexcept {
        return lilv_world_get_all_plugins (world);
    }

    void fill_features (std::vector<const LV2_Feature*>& fts) const noexcept {
        fts.push_back (symbols.map_feature());
        fts.push_back (symbols.unmap_feature());
        fts.push_back (&options_feature);
    }

private:
    friend class lvtk::World;
    friend class lvtk::Instance;
    friend class detail::Instance;
    friend class lvtk::InstanceUI;
    friend class detail::InstanceUI;

    LilvWorld* world { nullptr };
    Public& owner;
    Symbols symbols;
    OptionArray options;
    LV2_Feature options_feature;
    double sample_rate = 44100.0;
};

class Instance {
public:
    Instance (lvtk::Instance& i, lvtk::World& w)
        : owner (i), world (w) {}
    ~Instance() {}

    bool loaded() const noexcept { return instance != nullptr; }

    void fill_info (bool load_uis = false) {
        if (! info.URI.empty())
            return;

        lilv::Node uri (lilv_plugin_get_uri (plugin));
        lilv::Node name (lilv_plugin_get_name (plugin));
        lilv::Node author_email (lilv_plugin_get_author_email (plugin));
        lilv::Node author_name (lilv_plugin_get_author_name (plugin));
        lilv::Node author_homepage (lilv_plugin_get_author_homepage (plugin));
        lilv::Node bundle_uri (lilv_plugin_get_bundle_uri (plugin));

        info.URI             = uri.as_string();
        info.name            = name.as_string();
        info.author_email    = author_email.as_string();
        info.author_homepage = author_homepage.as_string();
        info.author_name     = author_name.as_string();

        if (load_uis)
            info.uis = detail::supported_uis (world.impl->world, plugin);
    }

private:
    friend class lvtk::Instance;
    friend class lvtk::World;
    friend class detail::World;
    friend class lvtk::InstanceUI;
    friend class detail::InstanceUI;

    lvtk::Instance& owner;
    lvtk::World& world;
    PluginInfo info;
    const LilvPlugin* plugin { nullptr };
    LilvInstance* instance { nullptr };
};

} // namespace detail

Instance::Instance (World& w) {
    impl = std::make_unique<detail::Instance> (*this, w);
}

Instance::~Instance() {
    impl.reset();
}

const std::string& Instance::name() const noexcept { return info().name; }
const PluginInfo& Instance::info() const noexcept {
    impl->fill_info (false);
    return impl->info;
}

std::unique_ptr<InstanceUI> Instance::instantiate_ui (uintptr_t parent) const noexcept {
    std::unique_ptr<InstanceUI> ui;
    const auto& supported = impl->info.uis;
    if (supported.empty())
        return ui;

    auto info = supported.front();

    ui.reset (new InstanceUI (impl->world, *const_cast<Instance*> (this)));
    ui->impl->set_parent (parent);
    lilv::UIs luis (lilv_plugin_get_uis (impl->plugin));
    lilv::Node uri (lilv_new_uri (impl->world.impl->world, info.URI.c_str()));
    auto lui              = lilv_uis_get_by_uri (luis, uri);
    ui->impl->info        = info;
    ui->impl->plugin_URI  = lilv_node_as_uri (lilv_plugin_get_uri (impl->plugin));
    ui->impl->bundle_path = lilv_file_uri_parse (lilv_node_as_uri (lilv_ui_get_bundle_uri (lui)), nullptr);
    ui->impl->binary_path = lilv_file_uri_parse (lilv_node_as_uri (lilv_ui_get_binary_uri (lui)), nullptr);
    ui->impl->instantiate();

    if (! ui->impl->loaded())
        ui.reset();

    return ui;
}

void Instance::write (uint32_t port, uint32_t size, uint32_t protocol, const void* data) {}

Handle Instance::handle() const noexcept { return lilv_instance_get_handle (impl->instance); }

void Instance::activate() { lilv_instance_activate (impl->instance); }
void Instance::connect_port (uint32_t port, void* data) { lilv_instance_connect_port (impl->instance, port, data); }
void Instance::run (uint32_t nframes) { lilv_instance_run (impl->instance, nframes); }
void Instance::deactivate() { lilv_instance_deactivate (impl->instance); }

World::World() {
    impl = std::make_unique<detail::World> (*this);
}

World::~World() {
    impl.reset();
}

void World::load_all() { impl->load_all(); }

std::vector<PluginInfo> World::plugins() const noexcept {
    std::vector<PluginInfo> vec;
    auto plugs = impl->plugins();
    LILV_FOREACH (plugins, i, plugs) {
        Instance instance (*const_cast<lvtk::World*> (this));
        instance.impl->plugin = lilv_plugins_get (plugs, i);
        instance.impl->fill_info();
        vec.push_back (instance.impl->info);
    }

    return vec;
}

std::vector<std::string> World::plugin_uris() const noexcept {
    std::vector<std::string> vec;
    auto plugs = lilv_world_get_all_plugins (impl->world);
    LILV_FOREACH (plugins, i, plugs) {
        auto plug = lilv_plugins_get (plugs, i);
        lilv::Node uri (lilv_plugin_get_uri (plug));
        vec.push_back (uri.as_string());
    }
    return vec;
}

std::vector<const LV2_Feature*> World::features() const noexcept {
    std::vector<const LV2_Feature*> fts;
    impl->fill_features (fts);
    return fts;
}

std::unique_ptr<Instance> World::instantiate (const std::string& uri) const noexcept {
    std::unique_ptr<Instance> inst;
    auto nuri = lilv::Node (lilv_new_uri (impl->world, uri.c_str()));

    if (auto plugin = lilv_plugins_get_by_uri (impl->plugins(), nuri)) {
        auto fs = features();
        fs.push_back (nullptr);
        if (auto lpi = lilv_plugin_instantiate (plugin, impl->sample_rate, fs.data())) {
            inst.reset (new Instance (*const_cast<World*> (this)));
            inst->impl->plugin   = plugin;
            inst->impl->instance = lpi;
            inst->impl->info.uis = detail::supported_uis (impl->world, plugin);
        } else {
            // no instantiate
        }
    } else {
        // not found
    }

    if (inst != nullptr) {
        // post setup
    }

    return inst;
}

void World::set_sample_rate (double rate) {
    impl->sample_rate = rate;
}

} // namespace lvtk
