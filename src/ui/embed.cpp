

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

    Window (Main& m, Embed& x) : main (m), owner (x) {
        parent = x.find_view();
        if (parent == nullptr) {
            std::clog << "connecting to sig....\n";
            conn_elevated = x.__sig_elevated().connect (
                std::bind (&Window::embed_container_elevated, this));
        }
        create_window();
    }

    ~Window() {
        conn_elevated.disconnect();
    }

    void create_window() {
        if (proxy != nullptr)
            return;

        proxy = std::make_unique<Proxy>();
        proxy->set_size (360, 240);
        proxy->set_visible (true);
        main.elevate (*proxy, parent != nullptr ? parent->handle() : 0);
        if (parent != nullptr) {
            std::clog << "[embed] parent found on instantiate. set visible true\n";
            proxy->set_visible (true);
        }

        assert (proxy->find_view().valid());
        assert (proxy->elevated());
    }

    std::unique_ptr<Proxy> proxy;

    boost::signals2::connection conn_elevated;
    void embed_container_elevated() {
        std::clog << "[embed] window: ancestor elevated\n";
        conn_elevated.disconnect();
    }
};

Embed::Embed (Main& main)
    : window (std::make_unique<Window> (main, *this)) {
    // should be opaque
}

Embed::~Embed() {
    window.reset();
}

void Embed::paint (Graphics& g) {
    g.set_color (0x222222ff);
    g.fill_rect (bounds().at (0));
}

void Embed::resized() {
}

ViewRef Embed::host_view() const noexcept {
    return (window != nullptr && window->proxy != nullptr)
               ? window->proxy->find_view()
               : nullptr;
}

} // namespace lvtk
