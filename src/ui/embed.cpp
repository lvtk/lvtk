

#include <lvtk/ui/embed.hpp>
#include <lvtk/ui/main.hpp>

namespace lvtk {

struct Embed::Window {
    Main& main;
    Embed& owner;
    ViewRef parent;

    class Proxy : public lvtk::Widget {
    public:
        Proxy() {}
        ~Proxy() {}
    };

    Window (Main& m, Embed& x)
        : main (m), owner (x) {
        create_window();
    }

    ~Window() {
    }

    void create_proxy() {
        proxy = std::make_unique<Proxy>();
        proxy->set_size (360, 240);
        proxy->set_visible (true);
    }

    void create_window() {
        if (proxy != nullptr)
            return;

        parent = owner.find_view();
        if (parent == nullptr)
            parent = main.find_view (owner);

        if (parent != nullptr) {
            create_proxy();
            // TODO: use Pugl's parenting facilities
            main.elevate (*proxy, parent->handle());
            proxy->set_visible (true);
            parent->set_size (100, 100);
        } else {
            std::clog << "[embed] window: didn't get parent view\n";
        }
    }

    std::unique_ptr<Proxy> proxy;
};

Embed::Embed (Main& main)
    : window (std::make_unique<Window> (main, *this)) {
    // should be opaque
}

Embed::~Embed() {
    window.reset();
}

ViewRef Embed::host_view() const noexcept {
    return (window != nullptr && window->proxy != nullptr)
               ? window->proxy->find_view()
               : nullptr;
}

void Embed::paint (Graphics& g) {
    g.set_color (0x222222ff);
    g.fill_rect (bounds().at (0));
}

void Embed::resized() {
}

void Embed::children_changed() {
    std::clog << "[embed] children_changed()\n";
    window->create_window();
}

void Embed::parent_structure_changed() {
    std::clog << "[embed] parent_structure_changed\n";
    if (auto v = find_view()) {
        std::clog << "[embed] find_fiew() != nullptr\n";
        window->create_window();
    } else {
        std::clog << "[embed] find_fiew() == nullptr\n";
    }
}

} // namespace lvtk
