.. default-domain:: cpp
.. highlight:: cpp
.. namespace:: lvtk

#####
Views
#####

A :class:`View` is an object that manages a Pugl view. Events from Pugl 
are received by the view then dispatched to the :class:`Widget` tree.
The root widget has ownership of it's view making it the real parent. 
You may  think of it as a window, though it may be embedded and not represent a 
top-level system window. [#f1]_

This seems backwards? Shouldn't a View own a Widget?. You would think so,
but structuring it this way makes it easier to deal with the non-global-staticness
of LVTK and the rest of LV2 architecture.

It also makes it possible for client code to only have to do two things
memory management-wise:

1) Ensure all Widgets get deleted.  Using them on the stack is recommended.
2) Ensure lvtk::Main stays alive until all Widgets are deleted.

*****
Types
*****

Currently, the only view types are those provided by Pugl.  One for 
**modules**, and one for a **standalone** applications.  However, moving 
forward there will  need to be trivial view types for things like 
PopUp menus, Main menus, hidden event views, etc...

**********
Dispatcher
**********

LVTK communicates with :class:`Widget` by dispatching events in the view received
from Pugl. :class:`Backends` are :class:`View` factories providing custom
functionality on a per-backend basis.

*********
Elevation
*********

Widgets are really the main object and own the view.  When a widget owns it's
own view it is considered ``elevated`` and is the root Widget displaying all
of the content in the Window.

.. rubric:: Footnotes

.. [#f1] MacOS has a strong distinction between
   `views <https://developer.apple.com/documentation/appkit/nsview>`_,
   which may be nested, and
   `windows <https://developer.apple.com/documentation/appkit/nswindow>`_,
   which may not.
   On Windows and X11, everything is a nestable window,
   but top-level windows are configured differently.
