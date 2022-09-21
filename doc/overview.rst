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
minimal LV2 plugin or UI.  Extension mixins are then used to implement LV2 features or
utilize host-side features.

************
Architecture
************

* Core library is header only templates and utilities.
* Base plugin, UI, and extensions use NO virtual methods, thus reducing overhead.
* Uses modern C++. A compiler supporting ``c++14`` or better is required.

***********
Terminology
***********

For clarity, here is a glossary of terms used throughout this manual.

**Module**
   `LV2 Context:`
      A loadable module such as an LV2 plugin, UI, Dynamic Manifest, etc...
   
   `Lua Context:`
      A literal Lua module that you ``require ('lvtk.ModuleName')`` in ``.lua`` code.

**Application**
   A standalone program. On windows this would be a runnable ``.exe``

**Host**
   A host `Application` which loads LV2 `Modules`.

**Wrapper**
   An LVTK class that wraps an LV2 C-primitive.

**Container**
   `LV2 Context:`
      An LVTK class that wraps an LV2 C-primitive in a STL container.

   `GUI Context:`
      An LVTK Widget sub-class which lays out children in a predictable way. (like a VBox or HBox)

.. toctree::
