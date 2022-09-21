LVTK is a library for writing LV2 Plugins and GUIs.
LVTK provides lightweight API to ease writing plugins and hosts with UI.

LVTK is particularly suitable for use in plugins or other loadable modules.
It has no implicit context or mutable static data,
so it may be statically linked and used multiple times in the same process.

LVTK has a modular design with a core library plus extras for hosting and GUIs.
The core library implements platform support, is header-only, and depends only 
on standard system libraries and LV2_.

MacOS, Windows, and X11 are currently supported as platforms.

Graphics backends in the UI library are built individually,
so applications can depend only on the APIs that they use.

LVTK includes a graphics backend for OpenGL_ by default, but other graphics 
APIs can be used by implementing a custom backend.

.. _OpenGL: https://www.opengl.org/
.. _LV2: https://lv2plug.in/
