
#include <lvtk/ui/nanovg.hpp>
#include "demo.hpp"

int main (int argc, char** argv) {
    lvtk::Main context (lvtk::Mode::PROGRAM, std::make_unique<lvtk::NanoVG>());
    return lvtk::demo::run (context, argc, argv);
}
