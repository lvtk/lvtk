
#include <lvtk/ui/entry.hpp>
#include <lvtk/ui/slider.hpp>
#include <lvtk/ui/widget.hpp>

#include "utils.hpp"

namespace lvtk {
namespace demo {

class EntryDemo : public Widget {
public:
    EntryDemo() {
        add (entry);
        show_all();
        set_size (640, 360);
    }

    ~EntryDemo() {}

private:
    void update_text (Slider& slider) {
    }

    void resized() override {
        entry.set_bounds (10, 10, 120, 30);
    }

    void paint (Graphics& g) override {
        g.set_color (0x777777ff);
        g.fill_rect (bounds().at (0, 0));
    }

private:
    Entry entry;
};

std::unique_ptr<Widget> create_entry_demo() {
    return std::make_unique<EntryDemo>();
}

} // namespace demo
} // namespace lvtk
