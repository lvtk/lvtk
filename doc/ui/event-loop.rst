.. default-domain:: cpp
.. highlight:: cpp
.. namespace:: lvtk

----------
Event Loop
----------

LVTK does not contain any threads or other event loop "magic".
For flexibility, the event loop is driven manually by repeatedly calling :func:`Main::loop`,
which processes events from the window system and dispatches them to views when necessary.

The exact use of :func:`Main::loop` depends on the application.
Plugins typically call it with a ``timeout`` of 0 in a callback driven by the host.
This avoids blocking the main loop,
since other plugins and the host itself need to run as well.

A program can use whatever timeout is appropriate:
event-driven applications may wait forever by using a ``timeout`` of -1,
while those that draw continuously may use a significant fraction of the frame period
(with enough time left over to render widgets).
