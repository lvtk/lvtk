
#pragma once

#include <lvtk/ui/widget.hpp>

namespace lvtk {

class Main;

class Embed final : public Widget {
public:
    Embed (Main& main);
    ~Embed();

    ViewRef host_view() const noexcept;

    void paint (Graphics& g) override;
    void resized() override;

protected:
    void children_changed() override;
    void parent_structure_changed() override;

private:
    class Window;
    std::unique_ptr<Window> window;
};

} // namespace lvtk
