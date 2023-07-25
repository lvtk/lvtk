// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <lilv/lilv.h>

#include <lvtk/lvtk.hpp>

namespace lvtk {

class Model;
class World;

/** A LV2 plugin instance.
    Powered by Lilv
    @ingroup host
    @headerfile lvtk/host/instance.hpp
*/
class LVTK_API Instance final {
public:
    ~Instance() = default;

    /** Returns this Plugin's name. */
    inline std::string uri() const noexcept {
        return _instance != nullptr ? lilv_instance_get_uri (_instance) : "";
    }

    inline const void* extension_data (const std::string& uri) const noexcept {
        return _instance != nullptr ? lilv_instance_get_extension_data (_instance, uri.c_str())
                                    : nullptr;
    }

    inline void activate() noexcept {
        if (_instance != nullptr)
            lilv_instance_activate (_instance);
    }

    inline void connect_port (uint32_t port, void* data) noexcept {
        lilv_instance_connect_port (_instance, port, data);
    }

    inline void run (uint32_t nframes) noexcept {
        lilv_instance_run (_instance, nframes);
    }

    inline void deactivate() noexcept {
        if (_instance != nullptr)
            lilv_instance_deactivate (_instance);
    }

    const LV2_Descriptor* descriptor() const noexcept {
        return lilv_instance_get_descriptor (_instance);
    }

    inline Handle handle() const noexcept {
        return lilv_instance_get_handle (_instance);
    }

    inline operator bool() const noexcept { return _instance != nullptr; }

private:
    friend class Model;
    friend class World;
    Instance (World&, Model&, intptr_t) {}
    LilvInstance* _instance { nullptr };

    uint32_t port_index (const char*) { return 0; }
    void write (uint32_t port, uint32_t size, uint32_t protocol, const void* data) {}
};

} // namespace lvtk
