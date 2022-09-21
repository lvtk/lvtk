.. default-domain:: cpp
.. highlight:: cpp
.. namespace:: lvtk

#######
Plugins
#######

Writing an LV2 Plugin

Plugin descriptors are registered on the stack at the global scope. First
make a sublcass of :struct:`lvtk::Plugin <plugin>`, then register it with :struct:`lvtk::Descriptor`.

.. code-block:: cpp

    static lvtk::Descriptor<MyPlugin> my_plugin (
        "http://myplugin/uri",  //< MyPlugin's URI String
        {
            LV2_URID__map,      //< List of required host features
            LV2_WORKER__schedule
        }
    );
