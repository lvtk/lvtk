
#include <lvtk/ui/opengl.hpp>
#include "demo.hpp"

int main (int argc, char** argv) {
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>());
    return lvtk::demo::run (context, argc, argv);
}
