.. highlight:: cpp

###
UIs
###

LVTK provides a core UI template that implements boilerplate code needed
for an LV2 UI.  LV2 features are implemented via 
:doc:`extension mixin templates <extensions>`. Below is an overview of how this
works.  See the `C++ API Docs <api/classlvtk_1_1UI.html>`_ for details on UI
callbacks.

.. code-block:: cpp

   #include <lvtk/ui.hpp>

--------
Instance
--------

:class:`lvtk.UI` is a template base class for LV2 UI instances. 
Default implementations exist for most core callbacks, so you only have to 
implement :func:`widget()` and :func:`port_event()`.

The signature of subclass must match the one in ``lvtk/ui.hpp`` header. The
``args`` should be used in your constructor to get "details" about instantiation.

Since this is a template, simulated dynamic binding is used for the callbacks.
When using :class:`lvtk.Extension`  "extensions" no vtable lookups are invoked, 
like normal dynamic binding would.

Including the ``lvtk/ui.hpp`` header also adds ``lv2ui_descriptor`` so you don't
have to.

---------
Callbacks
---------

:func:`widget()`

    Get the LV2UI_Widget (required)

    It is ok to create your widget here, but make sure that it only gets allocated once.   

:func:`cleanup()`

    This is called immediately before the destructor. Override it if you
    need to do something special beforehand.

:func:`port_event()`

    Port events (optional)

    Called when port events are received from the host. Implement this to
    update the UI when properties change in the plugin.

:func:`write()`

    Write data to plugin ports.  Uses an internal :class:`lvtk.Controller`
    that implements LV2 standard methods of port writing.

:func:`cleanup()`

    Override this to handle cleanup. Is called immediately before the
    instance is deleted.  You only need to implement this if you'd like
    to do something special before the destructor.

----------
Descriptor
----------

UI descriptors are registered on the stack at the global scope. First
make a sublcass of :class:`lvtk::UI<MyUI>`, then register it 
with :class:`lvtk::UIDescriptor`.

.. code-block:: cpp

    static lvtk::UIDescriptor<MyUI> myui (
        "http://myplugin/uri/ui",  //< MyUI's URI String
        {
            LV2_URID__map,      //< List of required host features
            LV2_WORKER__schedule
        }
    );

When the descriptor is added to the global stack. LVTK internally registers
it to automatically be returned in LV2's ``lv2_ui_descriptor()`` entry function.

--------
Examples
--------

**Copy Audio**
Example without Extensions

.. code-block:: cpp

    #include <lvtk/plugin.hpp>
    #include <cstring>

    class CopyAudioUI : public lvtk::Plugin<CopyAudioUI>
    {
    public:
        CopyAudioUI (const Args& args) : lvtk::UI (args) { }

        LV2UI_Widget widget (uint32_t port, void* data) {
            // pseudo widget creation code.
            auto widget = create_my_widget_somehow();
            return (LV2UI_Widget) widget->native_handle();
        }

        void port_event (uint32_t port, uint32_t size, uint32_t format, const void* data) {
            // update GUI state
        }
    };

    // Register a descriptor for this plugin type
    static Descriptor<CopyAudio> copyaudio ("https://lvtk.org/plugins/CopyAudio");
