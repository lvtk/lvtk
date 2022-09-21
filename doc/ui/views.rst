.. default-domain:: cpp
.. highlight:: cpp
.. namespace:: lvtk

#####
Views
#####

A `view` is an object that receives events from Pugl and dispatches them
to `Widget`. You may think of it as a window,
though it may be embedded and not represent a top-level system window. [#f1]_

LVTK communicates with views by dispatching events.
For flexibility, the event handler can be a different object than the view.
This allows using :class:`View` along with a separate event handler class.
Alternatively, a view class can inherit from :class:`View` and set itself as its event handler,
for a more object-oriented style.

This documentation will use the latter approach,
so we will define a class for our view that contains everything needed:

.. code-block:: cpp

   class MyView : public lvtk::View
   {
   public:
      explicit MyView (pugl::World& world)
            : pugl::View{world}
      { }

     pugl::Status onEvent(const pugl::ConfigureEvent& event) noexcept;
     pugl::Status onEvent(const pugl::ExposeEvent& event) noexcept;

     // With other handlers here as needed...

     // Fallback handler for all other events
     template<LVTKEventType t, class Base>
     pugl::Status onEvent(const pugl::Event<t, Base>&) noexcept
     {
          return pugl::Status::success;
     }

   private:
      // Some data...
   };

LVTK will call an ``onEvent`` method of the event handler (the view in this case) for every event.

Note that LVTK uses a static dispatching mechanism rather than virtual functions to minimize overhead.
It is therefore necessary for the final class to define a handler for every event type.
A terse way to do this without writing every implementation is to define a fallback handler as a template,
as in the example above.
Alternatively, you can define an explicit handler for each event that simply returns :enumerator:`Status::success`.
This way, it will be a compile error if any event is not explicitly handled.

*********************
Configuring the Frame
*********************

Before display,
the necessary :doc:`frame <api/frame>` and :doc:`window <api/window>` attributes should be set.
These allow the window system (or plugin host) to arrange the view properly.

Derived classes can configure themselves during construction,
but we assume here that configuration is being done outside the view.
For example:

.. code-block:: cpp

   const double defaultWidth = 1920.0;
   const double defaultHeight = 1080.0;

   view.setWindowTitle("My Window");
   view.setDefaultSize(defaultWidth, defaultHeight);
   view.setMinSize(defaultWidth / 4.0, defaultHeight / 4.0);
   view.setAspectRatio(1, 1, 16, 9);

There are also several :type:`hints <LVTKViewHint>` for basic attributes that can be set:

.. code-block:: cpp

   view.set_hint (lvtk::ViewFlag::RESIZABLE, true);

*********
Embedding
*********

To embed the view in another window,
you will need to somehow get the :type:`native view handle <lvtk::NativeView>` for the parent,
then set it with :func:`View::setParentWindow`.
If the parent is a LVTK view,
the native handle can be accessed with :func:`View::nativeView`.
For example:

.. code-block:: cpp

   view.setParentWindow(view, parent.getNativeView());

****************
Showing the View
****************

Once the view is configured, it can be "realized" with :func:`View::realize`.
This creates a "real" system view, for example:

.. code-block:: cpp

   pugl::Status status = view.realize();
   if (status != pugl::Status::success) {
     std::cerr << "Error realizing view: " << pugl::strerror(status) << "\n";
   }

Note that realizing a view can fail for many reasons,
so the return code should always be checked.
This is generally the case for any function that interacts with the window system.
Most functions also return a :enum:`Status`,
but these checks are omitted for brevity in the rest of this documentation.

A realized view is not initially visible,
but can be shown with :func:`View::show`:

.. code-block:: cpp

   view.show();

To create an initially visible view,
it is also possible to simply call :func:`View::show()` right away.
The view will be automatically realized if necessary.

.. rubric:: Footnotes

.. [#f1] MacOS has a strong distinction between
   `views <https://developer.apple.com/documentation/appkit/nsview>`_,
   which may be nested, and
   `windows <https://developer.apple.com/documentation/appkit/nswindow>`_,
   which may not.
   On Windows and X11, everything is a nestable window,
   but top-level windows are configured differently.
