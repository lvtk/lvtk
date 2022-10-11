// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <lvtk/ui/main.hpp>
#include <lvtk/ui/opengl.hpp>
#include <lvtk/ui/view.hpp>

#include "demo.hpp"

namespace lvtk {
namespace demo {

template <class Wgt>
static int run (lvtk::Main& context) {
    try {
        auto content = std::make_unique<Wgt>();
        context.elevate (*content, lvtk::View::RESIZABLE, 0);
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
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>());
    return lvtk::demo::run<lvtk::demo::Content> (context);
}
#else
int main (int argc, char** argv) {
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>());
    return lvtk::demo::run<lvtk::demo::Content> (context);
}
#endif
