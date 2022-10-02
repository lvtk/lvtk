
#include <lvtk/ui/button.hpp>

namespace lvtk {
namespace detail {

class Button {
public:
    Button (lvtk::Button& b) : owner (b) {}

private:
    lvtk::Button& owner;
    friend class lvtk::Button;
    bool toggled = false;
    bool down = false, over = false;

    void notify_clicked (const Event&) {
        if (owner.on_clicked)
            owner.on_clicked();
    }
};

class TextButton {
public:
    TextButton (lvtk::TextButton& o) : owner (o) {}

private:
    lvtk::TextButton& owner;
    friend class lvtk::TextButton;
    std::string text;
};

} // namespace detail

Button::Button() : impl (std::make_unique<detail::Button> (*this)) {}
Button::~Button() { impl.reset(); }

bool Button::toggled() const noexcept { return impl->toggled; }

void Button::toggle (bool toggled) {
    if (toggled == impl->toggled)
        return;
    impl->toggled = toggled;
    repaint();
}

void Button::paint (Graphics& g) {
    paint_button (g, impl->over, impl->down);
}

void Button::enter (const Event&) {
    if (impl->over)
        return;
    impl->over = true;
    repaint();
}

void Button::exit (const Event&) {
    if (! impl->over)
        return;
    impl->over = false;
    repaint();
}

void Button::pressed (const Event&) {
    if (! impl->down) {
        impl->down = true;
        repaint();
    }
}

void Button::released (const Event& ev) {
    if (! impl->down)
        return;

    impl->down = false;
    if (contains (ev.pos))
        impl->notify_clicked (ev);
    repaint();
}

TextButton::TextButton() : impl (std::make_unique<detail::TextButton> (*this)) {}
TextButton::TextButton (const String& text)
    : impl (std::make_unique<detail::TextButton> (*this)) {
    impl->text = text.str();
}

TextButton::~TextButton() { impl.reset(); }

String TextButton::text() const noexcept { return impl->text; }
void TextButton::set_text (const String& text) {
    if (text == impl->text)
        return;
    impl->text = text.str();
    repaint();
}

} // namespace lvtk
