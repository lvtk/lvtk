.. highlight:: cpp

#######
Plugins
#######

LVTK provides a core Plugin template that implements boilerplate code needed
for an LV2 plugin.  LV2 features are implemented via 
:doc:`extension mixin templates <extensions>`. Below is an overview of how this
works.  See the `C++ API Docs <api/classlvtk_1_1Plugin.html>`_ for more details
on plugin callbacks.

.. code-block:: cpp

   #include <lvtk/plugin.hpp>

--------
Instance
--------

:class:`lvtk.Plugin` is a template base class for LV2 plugin instances. 
Default implementations exist for most core callbacks, so you only have to 
implement :func:`connect_port()` and :func:`run()`.

The signature of subclass must match the one in ``lvtk/plugin.hpp`` header. The
``args`` should be used in your constructor to get "details" about instantiation.

Since this is a template, simulated dynamic binding is used for the callbacks.
When using :class:`lvtk.Extension`  "extensions" no vtable lookups are invoked, 
like normal dynamic binding would.

Including the ``lvtk/plugin.hpp`` header also adds ``lv2_descriptor`` so you don't
have to.

---------
Callbacks
---------

:func:`activate()`

    Override this function if you need to do anything on activation.
    This is always called before the host starts using the :func:`run()`
    function. You should reset your plugin to it's initial state here.
    
:func:`connect_port()`

    Override this to connect to your own port buffers.

    Remember that if you want your plugin to be realtime safe this function
    may not block, allocate memory or otherwise take a long time to return.

:func:`run()`

    This is the process callback which should fill all output port buffers.
    You most likely want to override it - the default implementation does
    nothing.

    Remember that if you want your plugin to be realtime safe, this function
    may not block, allocate memory or take more than `sample_count` time
    to execute.

:func:`deactivate()`

    Override this function if you need to do anything on deactivation.
    The host calls this when it does not plan to make any more calls to
    :func:`run()` unless it calls :func:`activate()` again.

:func:`cleanup()`

    Override this to handle cleanup. Is called immediately before the
    instance is deleted.  You only need to implement this if you'd like
    to do something special before the destructor.

----------
Descriptor
----------

Plugin descriptors are registered on the stack at the global scope. First
make a sublcass of :class:`lvtk::Plugin <plugin>`, then register it 
with :class:`lvtk::Descriptor`.

.. code-block:: cpp

    static lvtk::Descriptor<MyPlugin> my_plugin (
        "http://myplugin/uri",  //< MyPlugin's URI String
        {
            LV2_URID__map,      //< List of required host features
            LV2_WORKER__schedule
        }
    );

When the descriptor is added to the global stack. LVTK internally registers
it to automatically be returned in LV2's ``lv2_descriptor()`` entry function.

--------
Examples
--------

**Copy Audio**
Example without Extensions

.. code-block:: cpp

    #include <lvtk/plugin.hpp>
    #include <cstring>

    class CopyAudio : public lvtk::Plugin<CopyAudio>
    {
    public:
        CopyAudio (const Args& args) : lvtk::Plugin (args) { }

        void connect_port (uint32_t port, void* data) {
            audio[port] = data;
        }

        void run (uint32_t sample_count) {
            std::memcpy (audio[1], audio[0], sample_count * sizeof (float));
        }

    private:
        float* audio[2];
    };

    // Register a descriptor for this plugin type
    static Descriptor<CopyAudio> copyaudio ("https://lvtk.org/plugins/CopyAudio");

**Copy Audio with Logger**
Example with Extension

.. code-block:: cpp

    #include <lvtk/plugin.hpp>
    #include <lvtk/ext/log.hpp>

    #include <cstring>

    class CopyAudio : public lvtk::Plugin<CopyAudio, lvtk::Log>
    {
    public:
        CopyAudio (const Args& args) : lvtk::Plugin (args) { }

        void activate() {
            auto& log = logger();
            log << "[info] CopyAudio activated!";
        }

        void connect_port (uint32_t port, void* data) {
            audio[port] = data;
        }

        void run (uint32_t sample_count) {
            std::memcpy (audio[1], audio[0], sample_count * sizeof (float));
        }

        void cleanup() {
            auto& log = logger();
            log << "[info] about to `delete` CopyAudio instance!";
        }

    private:
        float* audio[2];
    };

    // Register a descriptor for this plugin type
    static lvtk::Descriptor<CopyAudio> copyaudio ("https://lvtk.org/plugins/CopyAudioWithLogger");
