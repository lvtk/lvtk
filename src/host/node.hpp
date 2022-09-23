
#pragma once

#include <memory>
#include <lilv/lilv.h>
#include <lv2/ui/ui.h>

namespace lvtk {
namespace lilv {

struct NodeDeleter {
    void operator()(LilvNode* n) const {
        lilv_node_free (n);
    }
};

class Node : private std::unique_ptr<LilvNode, NodeDeleter> {
public:
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

    inline operator const LilvNode*() const noexcept { return get(); }
    
    bool is_float() const noexcept { return lilv_node_is_float (get()); }
    bool is_int() const noexcept { return lilv_node_is_int (get()); }
    bool is_string() const noexcept { return lilv_node_is_float (get()); }
    bool is_uri() const noexcept { return lilv_node_is_uri (get()); }

    float as_float (float fallback = 0.f) const noexcept { 
        return is_float() ? lilv_node_as_float (get()) : fallback; 
    }

    std::string as_string() const noexcept {
        return is_uri() ? lilv_node_as_uri(get()) :
                lilv_node_is_string(get()) ? lilv_node_as_string (get()) :
                lilv_node_is_literal(get()) ? lilv_node_as_string (get()) :
                "";
    }

    std::string as_uri() const noexcept {
        is_uri() ? lilv_node_as_uri (get()) : "";
    }


private:
    bool _owned {true};
};

}}
