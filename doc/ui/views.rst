.. default-domain:: cpp
.. highlight:: cpp
.. namespace:: lvtk

#####
Views
#####

A `view` is an object that receives events from Pugl and dispatches them
to it's `Widget`. You may think of it as a window,
though it may be embedded and not represent a top-level system window. [#f1]_

LVTK communicates with Widget by dispatching events in the view.
For flexibility, the event handler can be a different object than the view.
This allows using :class:`View` along with a separate event handler class.
Alternatively, a view class can inherit from :class:`View` and set itself as its event handler,
for a more object-oriented style.

.. rubric:: Footnotes

.. [#f1] MacOS has a strong distinction between
   `views <https://developer.apple.com/documentation/appkit/nsview>`_,
   which may be nested, and
   `windows <https://developer.apple.com/documentation/appkit/nswindow>`_,
   which may not.
   On Windows and X11, everything is a nestable window,
   but top-level windows are configured differently.
