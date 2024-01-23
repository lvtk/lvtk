// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <lvtk/ui/entry.hpp>

#include "ui/detail/pugl.hpp"

namespace lvtk {
namespace detail {
class Entry {
public:
    Entry (lvtk::Entry& o) : owner (o) {}

    void paint (Graphics& g) {
        g.set_color (0x222222ff);
        g.fill_rect (owner.bounds().at (0));

        g.set_color (0xffffffff);
        font = font.with_height (15.f);
        g.set_font (font);
        g.draw_text (current_text,
                     owner.bounds().at (0).smaller (2).as<float>(),
                     Justify::LEFT_MIDDLE);
    }

    bool text_entry (const TextEvent& ev) {
        auto osize = current_text.size();

        for (const auto c : ev.body) {
            if (c >= ' ' && c <= '~') {
                current_text += c;
                ++cursor;
            } else if (c == PUGL_KEY_BACKSPACE) {
                handle_backspace();
            } else if (c == PUGL_KEY_DELETE) {
                handle_delete();
            }
        }

        if (osize != current_text.size())
            owner.repaint();
        return true;
    }

    void handle_delete() {
        if (current_text.size() > cursor)
            current_text.erase (cursor, 1);
    }

    void handle_backspace() {
        current_text.pop_back();
        if (cursor > 0)
            --cursor;
    }

private:
    friend class lvtk::Entry;
    lvtk::Entry& owner;
    std::string current_text;
    uint32_t cursor = 0;
    lvtk::Font font;
};

} // namespace detail

Entry::Entry() : impl (std::make_unique<detail::Entry> (*this)) {}
Entry::~Entry() { impl.reset(); }

void Entry::pressed (const Event& ev) { grab_focus(); }
void Entry::paint (Graphics& g) { impl->paint (g); }
bool Entry::text_entry (const TextEvent& ev) { return impl->text_entry (ev); }

} // namespace lvtk
