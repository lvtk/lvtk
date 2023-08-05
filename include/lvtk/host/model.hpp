// Copyright 2023 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <lvtk/host/node.hpp>
#include <lvtk/host/port.hpp>
#include <lvtk/lvtk.hpp>

namespace lvtk {

class Instance;
class World;

/** A data model of an LV2 Plugin. 
    
    The Model provides data about an LV2 plugins.
*/
class Model final {
public:
    Model()  = default;
    ~Model() = default;

    Model (const Model& other) { operator= (other); }

    inline Model& operator= (const Model& o) {
        _plugin = o._plugin;
        return *this;
    }

    /** Returns true if this is a valid Model (not null). */
    inline bool valid() const noexcept { return _plugin != nullptr; }

    /** Create a runnable instance.
     
        @param samplerate The samplerate to use.
        @param features   List of features passed to plugin instance.
    */
    inline Instance* instantiate (double samplerate, const FeatureList& features) {
        if (_plugin == nullptr)
            return nullptr;

        if (auto cinst = lilv_plugin_instantiate (_plugin, samplerate, features)) {
            lvtk::ignore (cinst);
            // FIXME:
            // return std::unique_ptr<Instance> (new Instance (_data->world, *this, (intptr_t) cinst));
        }

        return nullptr;
    }

    //==============================================================================
    /** Returns the URI of this plugin. */
    inline std::string uri() const noexcept {
        if (_plugin == nullptr)
            return {};
        Node node (lilv_plugin_get_uri (_plugin));
        return node.as_uri();
    }

    inline std::string name() const noexcept {
        if (_plugin == nullptr)
            return {};
        Node node (lilv_plugin_get_name (_plugin));
        return node.as_string();
    }

    inline std::string author_name() const noexcept {
        if (_plugin == nullptr)
            return {};
        Node node (lilv_plugin_get_author_name (_plugin));
        return node.as_string();
    }

    //==========================================================================
    /** Returns the total number of ports reported. */
    inline uint32_t num_ports() const noexcept {
        return _plugin != nullptr ? lilv_plugin_get_num_ports (_plugin)
                                  : 0;
    }

    /** Returns a Port by index. */
    inline Port port_by_index (uint32_t index) const noexcept {
        return { _plugin, _plugin != nullptr ? lilv_plugin_get_port_by_index (_plugin, index) : nullptr };
    }

    //==========================================================================
    /** Returns all supported features, required and optional. */
    inline auto supported_features() const noexcept {
        return valid() ? features (lilv_plugin_get_supported_features, _plugin)
                       : std::vector<std::string>();
    }

    /** Returns required feature URIs. */
    inline auto required_features() const noexcept {
        return valid() ? features (lilv_plugin_get_required_features, _plugin)
                       : std::vector<std::string>();
    }

    /** Returns optional feature URIs. */
    inline auto optional_features() const noexcept {
        return valid() ? features (lilv_plugin_get_optional_features, _plugin)
                       : std::vector<std::string>();
    }

    inline operator bool() const noexcept { return valid(); }
    inline bool operator== (const Model& o) const noexcept { return _plugin == o._plugin; }
    inline bool operator!= (const Model& o) const noexcept { return _plugin != o._plugin; }

private:
    friend class World;
    Model (const LilvPlugin* plugin) : _plugin (plugin) {}
    const LilvPlugin* _plugin { nullptr };

    template <typename Ftfn>
    inline static std::vector<std::string> features (Ftfn&& getter, const LilvPlugin* plugin) noexcept {
        std::vector<std::string> vec;
        if (auto nodes = getter (plugin)) {
            vec.reserve (lilv_nodes_size (nodes));
            LILV_FOREACH (nodes, iter, nodes) {
                const Node node (lilv_nodes_get (nodes, iter));
                vec.push_back (node.as_uri());
            }

            lilv_nodes_free (nodes);
        }

        return vec;
    }
};

} // namespace lvtk

// SAVEME: re-incorporate in header only?
#if 0
class Model::Data final {
public:
    // clang-format off
    Data (World& w, Model& o, intptr_t p)
        : world (w), model (o), 
          plugin ((const LilvPlugin*) p), 
          num_ports (plugin != nullptr ? lilv_plugin_get_num_ports (plugin) : 0) {}
    // clang-format on

    ~Data() {}

private:
    friend class Model;
    World& world;
    Model& model;
    const LilvPlugin* plugin { nullptr };

    uint32_t num_ports = 0;
    std::vector<Port> ports;
};

const std::vector<Port>& Model::ports() const noexcept {
    auto& vec (_data->ports);

    if (valid() && vec.size() != num_ports()) {
        vec.clear();
        vec.reserve (num_ports());
        for (uint32_t i = 0; i < num_ports(); ++i) {
            Port port (_data->plugin, lilv_plugin_get_port_by_index (_data->plugin, i));
            vec.push_back (port);
        }

        std::sort (vec.begin(), vec.end(), [] (const Port& lhs, const Port& rhs) {
            return lhs.index() < rhs.index();
        });
    }

    return vec;
}
#endif
