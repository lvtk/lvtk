.. default-domain:: cpp
.. highlight:: cpp
.. namespace:: lvtk

####
Main
####

:class:`lvtk::Main` is the top-level context for GUIs which represents a 
running instance. Essentially it is a wrapper around PuglWorld with added 
functionality for displaying Widgets. It handles the connection to the 
window system, manages elevated :class:`lvtk::Widget` instances and drives 
the event loop.

An application or module typically has a single Main, which is constructed 
on startup and used to drive a modular UI or application.  The GUI subystem
**does not** use *any* global static data and therefore isn't your *typical* GUI
toolkit.

------------
Construction
------------

A context must be created before any views or widgets, and it must outlive 
all of its views. The constructor requires an argument to specify the context 
type:

.. code-block:: cpp

   lvtk::Main context { lvtk::Mode::PROGRAM, std::make_unique<lvtk::OpenGL>() };

For a plugin, specify :enumerator:`Mode::MODULE` instead.

.. include:: event-loop.rst
