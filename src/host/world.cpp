
#include <algorithm>
#include <iostream>

#include <lv2/ui/ui.h>

#include "node.hpp"

#include <lvtk/host/module.hpp>
#include <lvtk/host/world.hpp>

namespace lvtk {

namespace detail {
static unsigned ui_supported (const char*, const char*) { return 0; }

static SupportedUIs
    supported_uis (LilvWorld* world, const LilvPlugin* plugin) {
    SupportedUIs supported;

    LilvUIs* uis = lilv_plugin_get_uis (plugin);
    if (nullptr == uis)
        return supported;

    LILV_FOREACH (uis, iter, uis) {
        const LilvUI* lui = lilv_uis_get (uis, iter);

        SupportedUI sui;
        sui.show = false;
        sui.idle = false;

        // check for extension data
        {
            auto uri_node      = lilv::Node (lilv_new_uri (world, lilv_node_as_uri (lilv_ui_get_uri (lui))));
            auto ext_data_node = lilv::Node (lilv_new_uri (world, LV2_CORE__extensionData));
            auto show_node     = lilv::Node (lilv_new_uri (world, LV2_UI__showInterface));
            auto idle_node     = lilv::Node (lilv_new_uri (world, LV2_UI__idleInterface));

            if (auto* ext_nodes = lilv_world_find_nodes (world, uri_node, ext_data_node, nullptr)) {
                LILV_FOREACH (nodes, iter, ext_nodes) {
                    const auto* node = lilv_nodes_get (ext_nodes, iter);
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
                supported.push_back (sui);
                sui.URI       = lilv::Node (lilv_ui_get_uri (lui)).as_string();
                sui.container = LVTK_UI__NativeUI;
                sui.widget    = lilv_node_as_uri (uitype);
                supported.push_back (sui);
                continue;
            }
        }

        // no UI this far, check show interface
        if (sui.show) {
            sui.show      = true;
            sui.container = LV2_UI__showInterface;
            sui.widget    = LV2_UI__showInterface;
            supported.push_back (sui);
            continue;
        }
    }

    lilv_uis_free (uis);

    auto sorter = [] (const SupportedUI& lhs, const SupportedUI& rhs) { return lhs.quality > rhs.quality; };
    std::sort (supported.begin(), supported.end(), sorter);

    for (const auto& s : supported) {
        std::clog << "[lvtk] supported ui: " << s.URI << std::endl;
        std::clog << "[lvtk]    container: " << s.container << std::endl;
        std::clog << "[lvtk]       widget: " << s.widget << std::endl;
        std::clog << "[lvtk]         show: " << (int) s.show << std::endl;
    }

    return supported;
}
} // namespace detail

class InstanceImpl {
    Instance& module;
    std::vector<std::string> supportedUIs;
    LilvWorld* world;
    const LilvPlugin* plugin;
    const LilvUIs* uis;

public:
    InstanceImpl (Instance& m) : module (m) {}
    ~InstanceImpl() {}
};

Instance::Instance() {
    impl = std::make_unique<InstanceImpl> (*this);
}

Instance::~Instance() {
    impl.reset();
}

class InstanceUIImpl {
public:
    InstanceUIImpl(){}
    ~InstanceUIImpl(){}
};

class WorldImpl {
public:
    WorldImpl (World& o) : owner (o) {
        world = lilv_world_new();
    }

    ~WorldImpl() {
        if (world != nullptr)
            lilv_world_free (world);
        world = nullptr;
    }

    LilvWorld* world { nullptr };
    World& owner;
};

World::World() {
    impl = std::make_unique<WorldImpl> (*this);
}

World::~World() {
    impl.reset();
}

void World::load_all() {
    lilv_world_load_all (impl->world);
}

std::vector<PluginInfo> World::plugins() const noexcept {
    std::vector<PluginInfo> vec;
    auto plugs = lilv_world_get_all_plugins (impl->world);
    LILV_FOREACH (plugins, i, plugs) {
        auto plug = lilv_plugins_get (plugs, i);

        lilv::Node uri (lilv_plugin_get_uri (plug));
        lilv::Node name (lilv_plugin_get_name (plug));
        lilv::Node author_email (lilv_plugin_get_author_email (plug));
        lilv::Node author_name (lilv_plugin_get_author_name (plug));
        lilv::Node author_homepage (lilv_plugin_get_author_homepage (plug));
        lilv::Node bundle_uri (lilv_plugin_get_bundle_uri (plug));

        PluginInfo info;
        info.URI             = uri.as_string();
        info.name            = name.as_string();
        info.author_email    = author_email.as_string();
        info.author_homepage = author_homepage.as_string();
        info.author_name     = author_name.as_string();

        vec.push_back (info);
    }
    Instance mod;
    mod.impl.reset (new InstanceImpl (mod));
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

} // namespace lvtk
