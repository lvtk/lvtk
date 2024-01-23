// Copyright 2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <cstdio>
#include <functional>

#include <boost/algorithm/string/trim.hpp>

#include "demo.hpp"
#include <lvtk/ui/button.hpp>
#include <lvtk/ui/image.hpp>

namespace lvtk {
namespace demo {
namespace res {
#include "lv2_logo.png.h"
} // namespace res

class Images : public DemoWidget {
public:
    Images() {
        add (button1);
        button1.set_text ("Open File");
        button1.set_name ("Open File Button");
        button1.set_visible (true);
        button1.on_clicked = [this]() {
            choose_image();
            repaint();
        };

        add (button2);
        button2.set_name ("Previous Button");
        button2.set_text ("Previous");
        button2.set_visible (true);
        button2.on_clicked = [this]() {
            if (_images.empty())
                return;
            if (_image == 0)
                _image = _images.size() - 1;
            else
                --_image;
            repaint();
        };

        add (button3);
        button3.set_name ("Next Button");
        button3.set_text ("Next");
        button3.set_visible (true);
        button3.on_clicked = [this]() {
            if (_images.empty())
                return;
            if (++_image >= _images.size())
                _image = 0;
            repaint();
        };
    }

    ~Images() {
        _images.clear();
    }

protected:
    void paint (Graphics& g) override {
        DemoWidget::paint (g);

        if (_images.empty()) {
            _images.push_back (Image::load (res::lv2_logo_png,
                                            res::lv2_logo_png_size));
        }

        auto r = bounds().at (0);
        r.slice_bottom (bottom_box_size);

        g.set_color (0xff090909);
        g.fill_rect (r);

        // image padding
        auto r2 = r.reduced (image_padding);
        // adjust for tiny window size.
        if (r2.empty() || std::min (r2.width, r2.height) < (image_padding * 4))
            r2 = r.reduced (1);

        if (_image < _images.size()) {
            auto i = _images[_image];
            g.draw_image (i, r2.as<float>(), Fitment::CENTERED | Fitment::ONLY_SHRINK);
        }
    }

    auto bottom_box() {
        return bounds().at (0).slice_bottom (bottom_box_size).smaller (4);
    }

    void resized() override {
        int pad = 4;
        auto r  = bottom_box();
        button1.set_bounds (r.slice_left (120));
        button3.set_bounds (r.slice_right (120));
        r.slice_right (pad);
        button2.set_bounds (r.slice_right (120));
    }

private:
    enum { bottom_box_size = 40,
           image_padding   = 10 };
    TextButton button1, button2, button3;
    std::vector<Image> _images;
    uint32_t _image = 0;

    void choose_image() {
#if __linux__
        FILE* fp = popen ("zenity --file-selection --title 'Select an Image'", "r");
        if (fp == nullptr) {
            perror ("Pipe returned a error");
        } else {
            char line[256];
            std::string filename;
            while (fgets (line, sizeof (line), fp)) {
                filename = line;
                break;
            }
            boost::trim (filename);
            if (auto i = Image::load (filename)) {
                _images.push_back (i);
                _image = _images.size() - 1;
                repaint();
            }
        }
#endif
    }
};

std::unique_ptr<Widget> create_images_demo() {
    return std::make_unique<Images>();
}

} // namespace demo
} // namespace lvtk
