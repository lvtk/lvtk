// Copyright 2023 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>
#include <string>

#include <lvtk/host/node.hpp>
#include <lvtk/lvtk.h>

namespace lvtk {

class Model;

/** LilvPort wrapper. */
class Port final {
public:
    ~Port() = default;

    /** Returns true if the underlying LilvPort and LilvPlugin are not null. */
    inline bool valid() const noexcept { return _plugin != 0 && _port != 0; }

    /** Returns this port's name. */
    inline std::string name() const noexcept {
        return Node (lilv_port_get_name (_plugin, _port)).as_string();
    }

    /** Returns the symbol of this port. */
    inline std::string symbol() const noexcept {
        return Node (lilv_port_get_symbol (_plugin, _port)).as_string();
    }

    /** Returns the index of this port. */
    inline uint32_t index() const noexcept {
        return lilv_port_get_index (_plugin, _port);
    }

    /** Check a port has a given Port class. */
    inline bool is_a (const Node& type) const noexcept {
        return lilv_port_is_a (_plugin, _port, type);
    }

    /** Check a port has multiple classes. */
    template <typename... Args>
    inline bool is_a (const Node& first, Args&&... args) const noexcept {
        return lilv_port_is_a (_plugin, _port, first) && is_a (args...);
    }

    inline operator bool() const noexcept { return valid(); }

    inline Port& operator= (const Port& o) {
        _plugin = o._plugin;
        _port   = o._port;
        return *this;
    }

private:
    friend class Model;
    Port() = delete;
    Port (const LilvPlugin* plugin, const LilvPort* port)
        : _plugin (plugin), _port (port) {}
    const LilvPlugin* _plugin { nullptr };
    const LilvPort* _port { nullptr };
};

} // namespace lvtk
