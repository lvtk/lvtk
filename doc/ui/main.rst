.. default-domain:: cpp
.. highlight:: cpp
.. namespace:: lvtk

########
GUI Main
########

:struct:`lvtk::Main` is the top-level object for GUIs which represents a 
running instance. It handles the connection to the window system, manages 
elevated :struct:`lvtk::Widget` instances and drives the event loop.

An application typically has a single Main, which is constructed once on 
startup and used to drive a modular UI or application.

************
Construction
************

A context must be created before any views, and it must outlive all of its views.
The constructor requires an argument to specify the application type:

.. code-block:: cpp

   lvtk::Main context { lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>() };

For a plugin, specify :enumerator:`Mode::MODULE` instead.

.. include:: event-loop.rst
   