// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lilv/lilv.h>
#include <lv2/ui/ui.h>
#include <memory>

namespace lvtk {
namespace lilv {
namespace detail {

struct WorldDeleter {
    void operator() (LilvWorld* w) const {
        lilv_world_free (w);
    }
};

struct NodeDeleter {
    void operator() (LilvNode* n) const {
        lilv_node_free (n);
    }
};

struct UIsDeleter {
    void operator() (LilvUIs* uis) const {
        lilv_uis_free (uis);
    }
};

} // namespace detail

class Node : private std::unique_ptr<LilvNode, detail::NodeDeleter> {
public:
    using unique  = std::unique_ptr<LilvNode, detail::NodeDeleter>;
    using pointer = unique::pointer;
    using unique::get;
    using unique::operator bool;

    Node (LilvNode* node) {
        reset (node);
        _owned = true;
    }

    Node (const LilvNode* node) {
        reset (const_cast<LilvNode*> (node));
        _owned = false;
    }

    ~Node() {
        if (! _owned)
            release();
        reset();
    }

    bool is_float() const noexcept { return lilv_node_is_float (get()); }
    bool is_int() const noexcept { return lilv_node_is_int (get()); }
    bool is_string() const noexcept { return lilv_node_is_float (get()); }
    bool is_uri() const noexcept { return lilv_node_is_uri (get()); }

    float as_float (float fallback = 0.f) const noexcept {
        return is_float() ? lilv_node_as_float (get()) : fallback;
    }

    std::string as_string() const noexcept {
        return is_uri() ? lilv_node_as_uri (get()) : lilv_node_is_string (get()) ? lilv_node_as_string (get())
                                                 : lilv_node_is_literal (get())  ? lilv_node_as_string (get())
                                                                                 : "";
    }

    std::string as_uri() const noexcept {
        return is_uri() ? lilv_node_as_uri (get()) : "";
    }

    inline operator const LilvNode*() const noexcept { return get(); }

private:
    bool _owned { true };
};

class UIs : private std::unique_ptr<LilvUIs, detail::UIsDeleter> {
public:
    using unique  = std::unique_ptr<LilvUIs, detail::UIsDeleter>;
    using pointer = unique::pointer;
    using unique::get;
    using unique::operator bool;

    UIs (const LilvPlugin* plugin) {
        reset (lilv_plugin_get_uis (plugin));
        _owned = true;
    }

    UIs (LilvUIs* uis) {
        reset (uis);
        _owned = true;
    }

    UIs (const LilvNode* node) {
        reset (const_cast<LilvNode*> (node));
        _owned = false;
    }

    ~UIs() {
        if (! _owned)
            release();
        reset();
    }

    operator const LilvUIs*() const noexcept { return get(); }

private:
    bool _owned = true;
};

} // namespace lilv
} // namespace lvtk
