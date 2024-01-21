// Copyright 2019-2024 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <iostream>

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/view.hpp>

#if LVTK_DEMO_CAIRO
#    include <lvtk/ui/cairo.hpp>
#else
#    include <lvtk/ui/opengl.hpp>
#endif

#include "demo.hpp"

namespace lvtk {
namespace demo {

template <class Wgt>
static int run (lvtk::Main& context) {
    try {
        auto content = std::make_unique<Wgt>();
        if (auto view = context.elevate (*content, lvtk::View::RESIZABLE, 0)) {
            view->set_position ((1920 / 2) - (view->bounds().width / 2),
                                (1080 / 2) - (view->bounds().height / 2));
        }

        while (true) {
            context.loop (-1.0);
            if (! context.running())
                break;
        }
    } catch (...) {
        std::clog << "[demo] fatal error in main loop\n";
        context.set_exit_code (-1);
    }

    std::clog << "[demo] exiting with code: " << context.exit_code() << std::endl;
    return context.exit_code();
}

} // namespace demo
} // namespace lvtk

#ifdef _WIN32
#    include <windows.h>
int WinMain (HINSTANCE hInstance,
             HINSTANCE hPrevInstance,
             LPSTR lpCmdLine,
             int nShowCmd) {
    lvtk::ignore (hInstance, hPrevInstance, lpCmdLine, nShowCmd);

    struct ClogBuf : public std::stringbuf {
        ~ClogBuf() { sync(); }
        int sync() {
            ::OutputDebugStringA (str().c_str());
            str (std::string()); // Clear the string buffer
            return 0;
        }
    } dbgbuf;

    auto clogbuf = std::clog.rdbuf (&dbgbuf);

#    if LVTK_DEMO_CAIRO
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::Cairo>());
#    else
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>());
#    endif

    auto ret = lvtk::demo::run<lvtk::demo::Content> (context);

    std::clog.rdbuf (clogbuf);
    return ret;
}
#else
int main (int argc, char** argv) {
#    if LVTK_DEMO_CAIRO
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::Cairo>());
#    else
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>());
#    endif
    return lvtk::demo::run<lvtk::demo::Content> (context);
}
#endif
