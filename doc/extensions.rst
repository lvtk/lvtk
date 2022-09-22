##########
Extensions
##########

LVTK plugin and UI templates should be extended with Extension mixins.  This
is how LV2 features are implemented automatically.

----
Atom
----
.. code-block:: cpp

   #include <lvtk/ext/atom.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/atom>`__

This isn't an extension mixin, but rather a set of classes which make working
with LV2 atoms, sequences, objects, and forging easier in an STL kind of way.  
See the `C++ API Docs <api/group__atom.html>`_ for complete details.

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Atom`
      - `Wrapper <api/structlvtk_1_1Atom.html>`__
      - N/A
    * - :class:`lvtk.Object`
      - `Wrapper <api/structlvtk_1_1Object.html>`__
      - N/A
    * - :class:`lvtk.Sequence`
      - `Container <api/structlvtk_1_1Sequence.html>`__
      - N/A
    * - :class:`lvtk.Forge`
      - `Wrapper <api/structlvtk_1_1Forge.html>`__
      - N/A
    * - :class:`lvtk.Vector`
      - `Container <api/structlvtk_1_1Vector.html>`__
      - N/A

-------
BufSize
-------
.. code-block:: cpp

    #include <lvtk/ext/bufsize.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/buf-size>`__

**Mixin Usage**

Use this extension to aquire buffer information from the host. It will add a
:func:`buffer_details` method to your Plugin.  The return value is a 
:class:`lvtk::BufferDetails` object containing information about minimum
block length, maximum block length, nominal block length, and sequence size.

.. code-block:: cpp

    class MyPlug : public lvtk::Plugin<MyPlug, lvtk::BufSize> {
    public:
        MyPlug (lvtk::Args& args) : lvtk::Plugin (args) {
            const auto& details = buffer_details();

            // setup plugin using details. members are optional
            // and only set if provided by the host. e.g....

            if (details.min && details.max) {
                // got min/max do something with them
            } else {
                // wasn't set...
            }
        }
    };

**Direct Usage**

You can also use :class:`lvtk::BufferDetails` directly if your plugin doesn't
use LVTK base templates.  It works hand-in-hand with URID Map and LV2 Options.

.. code-block:: cpp

    const LV2_Feature* map = find_map_feature (host_features);
    const LV2_Feature* options = find_options_feature (host_features);
    if (map && options) {
        lvtk::BufferDetails details;
        details.apply_options (map, options);
        // do something with buffer details
    }

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.BufSize`
      - `Extension <api/structlvtk_1_1BufSize.html>`_
      - N/A
    * - :class:`lvtk.BufferDetails`
      - `Utility <api/structlvtk_1_1BufferDetails.html>`_
      - N/A

-----------
Data Access
-----------
.. code-block:: cpp

   #include <lvtk/ext/data_access.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/data-access>`__

**Mixin Usage**

Use this extension in a UI to get extension data from a plugin. It will add a
:func:`data_access` method to your Plugin.  The return value is 
``const void*`` and can be nullptr.

.. code-block:: cpp

    class MyUI : public lvtk::UI<MyUI, lvtk::DataAccess> {
    public:
        MyUI (lvtk::Args& args) : lvtk::UI (args) {
            if (const void* plugin_data = data_access ("http://theplugin/feature#data"))
                handle_plugin_data (plugin_data);
        }
    };

**Direct Usage**

You can also use :class:`lvtk::ExtensionData` directly if your UI doesn't
use LVTK base templates.

.. code-block:: cpp

    if (const LV2_Feature* feature = find_data_access_feature (host_features)) {
        lvtk::ExtensionData ed;
        if (ed.set (feature))
            if (const void* plugin_data = ed.data_access ("http://theplugin/feature#data"))
                handle_plugin_data (plugin_data);
    }

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.DataAccess`
      - `Extension <api/structlvtk_1_1DataAccess.html>`__
      - N/A
    * - :class:`lvtk.ExtensionData`
      - `Utility <api/structlvtk_1_1ExtensionData.html>`__
      - N/A

---------------
Instance Access
---------------
.. code-block:: cpp

   #include <lvtk/ext/instance_access.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/instance-access>`__

**Mixin Usage**

Use this extension in a UI to get the plugin instance handle. It will add a
:func:`plugin_instance` method to your Plugin.  The return value is 
``lvtk::Handle`` and can be nullptr if not supported.

.. code-block:: cpp

    class MyUI : public lvtk::UI<MyUI, lvtk::InstanceAccess> {
    public:
        MyUI (lvtk::Args& args) : lvtk::UI (args) {
            if (lvtk::Handle instance = plugin_instance())
                handle_plugin_data (plugin_data);
        }
    };

**Direct Usage**

You can also use :class:`lvtk.InstanceHandle` directly if your UI doesn't
use LVTK base templates.

.. code-block:: cpp

    if (const LV2_Feature* feature = find_instance_feature (host_features)) {
        lvtk::InstanceHandle handle;
        if (handle.set (feature))
            if (lvtk::Handle instance = handle.get())
                process_instance_handle (instance);
    }

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.InstanceAccess`
      - `Extension <api/structlvtk_1_1InstanceAccess.html>`__
      - N/A
    * - :class:`lvtk.InstanceHandle`
      - `Utility <api/structlvtk_1_1InstanceHandle.html>`__
      - N/A

---
Log
---
.. code-block:: cpp

   #include <lvtk/ext/log.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/log>`__

**Mixin Usage**

Use this extension in a plugin or UI to log messages. It will add a
:func:`logger` method to your class.  The return value is a
:class:`lvtk.Logger`.

.. code-block:: cpp

    class MyPlug : public lvtk::Plugin<MyPlug, lvtk::Log> {
    public:
        MyPlug (lvtk::Args& args) : lvtk::Plugin (args) {
            uint32_t trace_urid = map_trace_urid();
            auto& log = logger();
            log.printf (trace_urid, "Hello world!");

            // You can also use operator << 
            log << "[info] MyPlug instantiated ok!";
        }
    };

**Direct Usage**

You can also use :class:`lvtk.Logger` directly if your plugin or UI doesn't
use LVTK base templates.

.. code-block:: cpp

    lvtk::Logger logger;
    logger.set (find_log_feature (host_features));
    logger << "Hello world!";

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Log`
      - `Extension <api/structlvtk_1_1Log.html>`__
      - N/A
    * - :class:`lvtk.Logger`
      - `Utility <api/structlvtk_1_1Logger.html>`__
      - N/A

----
MIDI
----

**Spec:** `<https://lv2plug.in/ns/ext/midi>`__

-----
Morph
-----

**Spec:** `<https://lv2plug.in/ns/ext/morph>`__

-------
Options
-------
.. code-block:: cpp

   #include <lvtk/ext/options.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/options>`__

**Mixin Usage**

Use this extension in a plugin or UI to utilize LV2 Options. It will add a
:func:`options` method to your class.  The return value is an array of option
:class:`lvtk.Option` pointers.

.. code-block:: cpp

    class MyPlug : public lvtk::Plugin<MyPlug, lvtk::Options> {
    public:
        MyPlug (lvtk::Args& args) : lvtk::Plugin (args) {
            lvtk::OptionArray opts (options());
            for (const auto& opt : opts) {
                // handle option
            }
        }
    };

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Option`
      - `Alias <api/group__options.html>`__
      - N/A
    * - :class:`lvtk.OptionsContext`
      - `Alias <api/group__options.html>`__
      - N/A
    * - :class:`lvtk.OptionsStatus`
      - `Alias <api/group__options.html>`__
      - N/A
    * - :class:`lvtk.OptionsData`
      - `Feature Data <api/structlvtk_1_1OptionsData.html>`__
      - N/A
    * - :class:`lvtk.Options`
      - `Extension <api/structlvtk_1_1Options.html>`__
      - N/A
    * - :class:`lvtk.OptionArray`
      - `Utility <api/classlvtk_1_1OptionArray.html>`__
      - N/A

-----------
Port Groups
-----------

**Spec:** `<https://lv2plug.in/ns/ext/port-groups>`__

---------------
Port Properties
---------------

**Spec:** `<https://lv2plug.in/ns/ext/port-props>`__

-------
Presets
-------

**Spec:** `<https://lv2plug.in/ns/ext/presets>`__

-----------
Resize Port
-----------
.. code-block:: cpp

   #include <lvtk/ext/resize_port.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/resize-port>`__

**Mixin Usage**

Use this extension in a plugin to deal with port resizing. It will add a
:func:`resize_port` method to your class.  The return value is an
``lvtk.ResizePortStatus``.  You must gracefully handle non sucess return
values.

.. code-block:: cpp

    class MyPlug : public lvtk::Plugin<MyPlug, lvtk::Options> {
    public:
        MyPlug (lvtk::Args& args) : lvtk::Plugin (args) {
            // initialization ....
        }

        void run (uint32_t nframes) {
            uint32_t port_that_needs_resized = 0;
            uint32_t new_port_size = 1024;
            auto err = resize_port (port_that_needs_resized, new_port_size);
            if (err != LV2_RESIZE_PORT_SUCCESS) {
                // could not handle resize. handle gracefully
            }
        }
    };

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.ResizePortStatus`
      - `Alias <api/group__resize__port.html>`__
      - N/A
    * - :class:`lvtk.ResizePort`
      - `Extension <api/structlvtk_1_1ResizePort.html>`__
      - N/A
    * - :class:`lvtk.PortResizer`
      - `Utility <api/structlvtk_1_1PortResizer.html>`__
      - N/A

-----
State
-----
.. code-block:: cpp

   #include <lvtk/ext/state.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/state>`__

**Mixin Usage**

Use this extension in a plugin to implement LV2 State saving and restoring. 
It will add :func:`save` and :func:`restore` callbacks to your class.  The host
will call these when it wants to save or restore your plugin's state.

The return values are ``lvtk.StateStatus``.  You must correctly return the 
status depending on success or not. Each method uses either a :class:`lvtk.StateStore` 
or :class:`lvtk.StateRetrive` function object to read/write key/pairs.

.. code-block:: cpp

    class MyPlug : public lvtk::Plugin<MyPlug, lvtk::State, lvtk::URID> {
    public:
        MyPlug (lvtk::Args& args) : lvtk::Plugin (args) {
            custom_property_key = map_uri (custom_property_key_uri);
            string_type = map_uri (LV2_ATOM__String);
        }

        StateStatus save (StateStore& store, uint32_t flags, const FeatureList& features) {
            return store (custom_property_key, custom_value, strlen (custom_value) + 1, string_type, 0);
        }

        StateStatus restore (StateRetrieve& retrieve, uint32_t flags, const FeatureList& features) {
            size_t size = 0;
            uint32_t type = 0;
            uint32_t flags = 0;
            auto value_read = (const char*) retrieve (custom_property_key, &size, &type, &flags);
            return strcmp (value_read, custom_value) == 0 ? LV2_STATE_SUCCESS : LV2_STATE_ERR_BAD_TYPE;
        }

    private:
        static constexpr const char* custom_property_key_uri = "http://mycustom.dev#key";
        uint32_t custom_property_key {0};
        static constexpr const char* custom_value = "Hello save!";
        static constexpr const uint32_t custom_value_size = 11;
        uint32_t string_type {0};
    };

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.StateFlags`
      - `Alias <api/group__state.html>`__
      - N/A
    * - :class:`lvtk.StateStatus`
      - `Alias <api/group__state.html>`__
      - N/A
    * - :class:`lvtk.StateRetrieve`
      - `Function <api/structlvtk_1_1StateRetrieve.html>`__
      - N/A
    * - :class:`lvtk.StateStore`
      - `Function <api/structlvtk_1_1StateStore.html>`__
      - N/A
    * - :class:`lvtk.State`
      - `Extension <api/structlvtk_1_1State.html>`__
      - N/A

----
Time
----

**Spec:** `<https://lv2plug.in/ns/ext/time>`__

-----
Units
-----

**Spec:** `<https://lv2plug.in/ns/extensions/units>`__

----
URID
----
.. code-block:: cpp
    
    #include <lvtk/ext/urid.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/urid>`__

**Mixin Usage**

Use this extension in a plugin or UI to map and unmap URIs. It will add a
:func:`map_uri` and :func:`unmap_urid` methods to your class.  The return
values are either a mapped URID uint, or an unmapped std::string.

.. code-block:: cpp

    class MyPlug : public lvtk::Plugin<MyPlug, lvtk::Options> {
    public:
        MyPlug (lvtk::Args& args) : lvtk::Plugin (args) {
            uint32_t a_urid = map_uri ("http://auri-to.com#map");
            std::clog << unmap_urid (a_urid);
        }
    };

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Map`
      - `FeatureData <api/structlvtk_1_1Map.html>`__
      - N/A
    * - :class:`lvtk.Unmap`
      - `FeatureData <api/structlvtk_1_1Unmap.html>`__
      - N/A
    * - :class:`lvtk.URID`
      - `Extension <api/structlvtk_1_1URID.html>`__
      - N/A
    * - :class:`lvtk.Symbols`
      - `Utility <api/classlvtk_1_1Symbols.html>`__
      - N/A

------
Worker
------
.. code-block:: cpp

   #include <lvtk/ext/worker.hpp>

**Spec:** `<https://lv2plug.in/ns/ext/worker>`__

**Mixin Usage**

Use this extension in a plugin to implment LV2 Worker. It will add a
:func:`schedule_work` method to your class. Callbacks :func:`work`,
:func:`work_response`, :func:`end_run` will be called by the host according
to LV2 Worker specifications.  The return values are ``lvtk.WorkStatus``
values.

.. code-block:: cpp

    class WorkHorse : public lvtk::Plugin<WorkHorse, lvtk::Worker> {
    public:
        WorkHorse (const lvtk::Args& args) : lvtk::Plugin (args) {}

        void run (uint32_t nframes) {
            // Offload some non-realtime work.
            // This just writes a string which isn't that useful except 
            // for example
            schedule_work ("workmsg", strlen("workmsg") + 1);
        }

        WorkerStatus work (WorkerRespond &respond, uint32_t size, const void* data) {
            // The host has recieved your request and is calling work in another thread.
            // Send a response back to the audio thread.
            return respond (strlen("work_ack") + 1, "work_ack");
        }

        WorkerStatus work_response (uint32_t size, const void* body) {
            // handle responses sent in @c work
            // A real plugin would probably not just be logging a string body....
            std::clog << (const char*)body << std::endl;
            return LV2_WORKER_SUCCESS;
        }

        WorkerStatus end_run() {
            // optional: do anything needed at the end of the run cycle
            // Work responses have been handled, no perform any end_run
            // activies here (audio thread)
        }
    };

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.WorkerStatus`
      - `Alias <api/group__worker.html>`__
      - N/A
    * - :class:`lvtk.WorkerRespond`
      - `Function <api/structlvtk_1_1WorkerRespond.html>`__
      - N/A
    * - :class:`lvtk.Worker`
      - `Extension <api/structlvtk_1_1Worker.html>`__
      - N/A

----
UI
----

UI specific extensions work the same as Plugin extensions.  There are a 
handful of them and are all under the same `UI` specification.

**Spec:** `<https://lv2plug.in/ns/extensions/ui>`__

----
Idle
----
.. code-block:: cpp
    
    #include <lvtk/ext/ui/idle.hpp>

**Mixin Usage**

Adds :func:`idle()` callback to your UI class. Called repeatedly by the host 
to drive your UI.  Return non-zero to stop receiving callbacks.

.. code-block:: cpp

    class MyUI : public lvtk::UI<MyUI, lvtk::Idle> {
    public:
        MyUI (const lvtk::Args& args) : lvtk::UI (args) {
        }

         int idle() {
            // drive the event loop.
            return 0; // keep going!
        }
    };

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Idle`
      - `Extension <api/structlvtk_1_1Idle.html>`__
      - N/A

------
Parent
------
.. code-block:: cpp
    
    #include <lvtk/ext/ui/parent.hpp>

Adds a :func:`parent` function object to use. It returns the parent widget, 
or nullptr if not provided.
It is a function object and also has a bool() operator, so....

.. code-block:: cpp

    class MyUI : public lvtk::UI<MyUI, lvtk::Parent> {
    public:
        MyUI (const lvtk::Args& args) : lvtk::UI (args) {
            // ... Inside your UI's constructor ....

            if (parent) {
                auto* widget = reinterpret_cast<WidgetType*> (parent())
                // do something with the parent widget, WidgetType* above
                // would be an object of whatever toolkit you're using.
            }
        }
    };

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Parent`
      - `Extension <api/structlvtk_1_1Parent.html>`__
      - N/A

--------
Port Map
--------
.. code-block:: cpp
    
    #include <lvtk/ext/ui/port_map.hpp>

**Mixin Usage**

Adds a :func:`port_index` method to your UI.  Call it to get a port's index
from it's symbol.

.. code-block:: cpp

    class MyUI : public lvtk::UI<MyUI, lvtk::PortMap> {
    public:
        MyUI (const lvtk::Args& args) : lvtk::UI (args) {
            auto audio_port_1_index = port_map ("audio_01");
            // use the index how you see fit.
        }

        void port_event (uint32_t port, uint32_t size, uint32_t format, const void* data) {
            // handle port notifications.
        }

        void cleanup() {
            unsubscribe (port_index, protocol, unsubscribe_features);
        }

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.PortMap`
      - `Extension <api/structlvtk_1_1PortMap.html>`__
      - N/A

--------------
Port Subscribe
--------------
.. code-block:: cpp
    
    #include <lvtk/ext/ui/port_subscribe.hpp>

**Mixin Usage**

Adds :func:`subscribe` and :func:`unsubscribe` methods to your UI.  Call them
to start or stop receiving notifications about ports in ``port_event``.

.. code-block:: cpp

    class MyUI : public lvtk::UI<MyUI, lvtk::PortSubscribe> {
    public:
        MyUI (const lvtk::Args& args) : lvtk::UI (args) {
            subscribe (port_index, protocol, subscribe_features);
        }

        void port_event (uint32_t port, uint32_t size, uint32_t format, const void* data) {
            // handle port notifications.
        }

        void cleanup() {
            unsubscribe (port_index, protocol, unsubscribe_features);
        }

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.PortSubscribe`
      - `Extension <api/structlvtk_1_1PortSubscribe.html>`__
      - N/A

----
Show
----
.. code-block:: cpp
    
    #include <lvtk/ext/ui/show.hpp>

**Mixin Usage**

Adds :func:`show` and :func:`hide` callbacks to your UI.  It inherrits from
:class:`lvtk.Idle` so don't use Idle & Show together.... just use :class:`lvtk.Show`

.. code-block:: cpp

    class MyUI : public lvtk::UI<MyUI, lvtk::Show> {
    public:
        MyUI (const lvtk::Args& args) : lvtk::UI (args) {}

        int show() {
            // host is requesting you show your GUI
        }

        int hide() {
            // host is requesting you hide your GUI
        }

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Show`
      - `Extension <api/structlvtk_1_1Show.html>`__
      - N/A

-----
Touch
-----
.. code-block:: cpp
    
    #include <lvtk/ext/ui/touch.hpp>

**Mixin Usage**

Adds a :func:`touch()` method to your UI class.  Use it to notify the host 
about gesture changes.

.. code-block:: cpp

    class MyUI : public lvtk::UI<MyUI, lvtk::Touch> {
    public:
        MyUI (const lvtk::Args& args) : lvtk::UI (args) {
        }

        void send_gesture_change() {
            auto port_index = 0;
            auto grabbed = check_grabbed();
            touch (port_index, grabbed);
        }
    };

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Touch`
      - `Extension <api/structlvtk_1_1Touch.html>`__
      - N/A
