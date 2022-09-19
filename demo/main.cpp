
#include <lvtk/ui/main.hpp>
#include <lvtk/ui/opengl.hpp>
#include <lvtk/ui/view.hpp>

#include "demo.hpp"

template <class Wgt>
static int run (lvtk::Main& context) {
    try {
        auto content = std::make_unique<Wgt> (context);
        context.elevate (*content, lvtk::ViewFlag::RESIZABLE, 0);
        bool quit = false;
        while (! quit) {
            context.loop (-1.0);
            quit = context.__quit_flag;
        }
    } catch (...) {
        std::clog << "fatal error in main loop\n";
        // std::clog << e.what() << std::endl;
        return 1;
    }

    return 0;
}

static int run (lvtk::Main& context, int, char**) {
    return run<lvtk::demo::Content> (context);
}

#ifdef _WIN32
#include <windows.h>
int WinMain (HINSTANCE hInstance,
             HINSTANCE hPrevInstance,
             LPSTR     lpCmdLine,
             int       nShowCmd) 
             {
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>());
    return run<lvtk::demo::Content> (context);
}
#else
int main (int argc, char** argv) {
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>());
    return run (context, argc, argv);
}
#endif
