// Copyright 2023 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>
#include <string>

#include <lilv/lilv.h>

namespace lvtk {
namespace detail {

/** @internal */
struct FreeNode {
    void operator() (LilvNode* n) const {
        lilv_node_free (n);
    }
};

} // namespace detail

/** A LilvNode wrapper. */
class Node final : private std::unique_ptr<LilvNode, detail::FreeNode> {
public:
    using unique  = std::unique_ptr<LilvNode, detail::FreeNode>;
    using pointer = unique::pointer;
    using unique::operator bool;

    /** Wraps a mutable LilvNode. The wrapped node will be free'd in the dtor.
     */
    Node (LilvNode* node) {
        reset (node);
        _owned = true;
    }

    /** Wraps a readonly LilvNode. The wrapped node will not be free'd in the
        dtor.
     */
    Node (const LilvNode* node) {
        reset (const_cast<LilvNode*> (node));
        _owned = false;
    }

    /** Copy ctor copies the node. */
    Node (const Node& node) { operator= (node); }

    /** Move support. */
    Node (Node&& node) {
        static_cast<unique&> (*this) = std::move (node);
        _owned                       = node._owned;
    }

    ~Node() {
        if (_owned) {
            reset();
        } else {
            release();
        }
    }

    inline const pointer get() const noexcept { return unique::get(); }

    /** Returns true if this is a float. */
    inline bool is_float() const noexcept { return lilv_node_is_float (get()); }
    /** Returns true if this is a float. */
    inline bool is_int() const noexcept { return lilv_node_is_int (get()); }
    /** Returns true if this is an integer. */
    inline bool is_string() const noexcept { return lilv_node_is_string (get()); }
    /** Returns true if this is a URI string. */
    inline bool is_uri() const noexcept { return lilv_node_is_uri (get()); }
    /** Returns true if blank. */
    inline bool is_blank() const noexcept { return lilv_node_is_blank (get()); }
    /** Returns true if a literal. */
    inline bool is_literal() const noexcept { return lilv_node_is_literal (get()); }

    inline float as_float (float fallback = 0.f) const noexcept {
        return is_float() ? lilv_node_as_float (get()) : fallback;
    }

    inline std::string as_string() const noexcept {
        // clang-format off
        return is_uri() ? lilv_node_as_uri (get()) 
            : lilv_node_is_string (get()) ? lilv_node_as_string (get())
            : lilv_node_is_literal (get()) ? lilv_node_as_string (get()) 
            : "";
        // clang-format on
    }

    inline std::string as_uri() const noexcept {
        return is_uri() ? lilv_node_as_uri (get()) : "";
    }

    inline bool operator== (const Node& o) const noexcept { return lilv_node_equals (get(), o.get()); }
    inline bool operator!= (const Node& o) const noexcept { return ! lilv_node_equals (get(), o.get()); }
    inline operator const pointer() const noexcept { return get(); }

    inline Node& operator= (const Node& node) {
        if (get() != nullptr) {
            if (_owned)
                reset();
            else
                release();
        }

        if (auto cptr = node.get())
            reset (lilv_node_duplicate (cptr));

        _owned = true;
        return *this;
    }

    inline Node& operator= (Node&& node) {
        _owned                       = std::move (node._owned);
        static_cast<unique&> (*this) = std::move (node);
        return *this;
    }

private:
    bool _owned { true };
};

} // namespace lvtk
