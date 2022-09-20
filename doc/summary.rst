LVTK is a library for writing LV2 Plugins and Guis.
LVTK is not a toolkit or framework,
but a minimal portability layer that sets up a drawing context and delivers events.

LVTK is particularly suitable for use in plugins or other loadable modules.
It has no implicit context or mutable static data,
so it may be statically linked and used multiple times in the same process.

LVTK has a modular design with a core library and separate graphics backends.
The core library implements platform support and depends only on standard system libraries.
MacOS, Windows, and X11 are currently supported as platforms.

Graphics backends are built as separate libraries,
so applications can depend only on the APIs that they use.
LVTK includes a graphics backend for OpenGL_, but other graphics APIs can be used by implementing a custom backend.

.. _OpenGL: https://www.opengl.org/
