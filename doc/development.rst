##########
Developers
##########

*********************
Building Against LVTK
*********************

When LVTK is installed,
pkg-config_ packages are provided that link with the UI library and desired backend:

.. code-block:: sh

   pkg-config --cflags --libs lvtk-3
   pkg-config --cflags --libs lvtk-ui-3
   pkg-config --cflags --libs lvtk-gl-3

Depending on one of these packages should be all that is necessary to use LVTK,
but details on the individual libraries that are installed are available in the README.

If you are instead building directly from source,
all of the implementation files are in the ``src`` directory.
It is only necessary to build the platform and backend implementations that you need.

****************
Meson Subproject
****************

LVTK can also be included as a `meson subproject <https://mesonbuild.com/Subprojects.html>`_.

*************
API Reference
*************

Looking for the API docs?

- `C++ API documentation <api/index.html>`_.
- `Lua API documentation <#>`_.

.. _pkg-config: https://www.freedesktop.org/wiki/Software/pkg-config/
