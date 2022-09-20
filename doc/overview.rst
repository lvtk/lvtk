.. default-domain:: cpp
.. highlight:: cpp
.. namespace:: lvtk

########
Overview
########

LVTK is a C++ library,
but the bindings documented here provide a more idiomatic and type-safe API for C++.
If you would rather use C,
refer instead to the `C++ API documentation <api/index.html>`_.

The C++ bindings are very lightweight and do not require virtual functions,
RTTI,
exceptions,
or linking to the C++ standard library.
They are provided by the package ``lvtk-3`` which must be used in addition to the desired platform+backend package above.

The core API (excluding backend-specific components) is declared in ``lvtk.hpp``:

.. code-block:: cpp

   #include <lvtk/lvtk.hpp>

The core API revolves around two main objects: the `Plugin` and the `UI`.
A developer uses these templates to create an LV2 plugin or UI.

.. toctree::

   world
   view
   events
   event-loop
