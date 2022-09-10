
#pragma once

#include <lvtk/ui/widget.hpp>

namespace lvtk {

class Main;

class Embed final : public Widget {
public:
    Embed (Main& main);
    ~Embed();

    void paint (Graphics& g) override;
    void resized() override;

    ViewRef host_view() const noexcept;

private:
    class Window;
    std::unique_ptr<Window> window;
};

} // namespace lvtk
