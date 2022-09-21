.. default-domain:: cpp
.. highlight:: cpp
.. namespace:: lvtk

########
Overview
########

LVTK is a C++ library for Plugin and GUI developers.
If you would rather see the reference docs, refer instead to 
the `C++ API documentation <api/index.html>`_.

The C++ Plugin, UI, extension, and utility templates are very lightweight and 
do not require virtual functions, RTTI, exceptions, or linking to the C++ standard 
library. They are provided by the package ``lvtk-3`` which must be used in addition 
to binary libraries if wanted.

The core API (excluding library-specific components) is declared in ``lvtk.hpp``:

.. code-block:: cpp

   #include <lvtk/lvtk.hpp>

The core API revolves around two main objects: the `Plugin <api/classlvtk_1_1Plugin.html>`_ 
and the `UI <api/classlvtk_1_1UI.html>`_. A developer uses these templates to create a 
minimal LV2 plugin or UI.

.. toctree::

   world
   view
   events
   event-loop
