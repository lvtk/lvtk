#######
Hosting
#######

-----
World
-----

A manager for the LV2 ecosystem. Powered by `Lilv <https://gitlab.com/lv2/lilv/>`__
and `Suil <https://gitlab.com/lv2/suil/>`__
A LV2 UI instance.  The :class:`lvtk.World` creates this via it's `instantiate`
method.  It is up to client code what to do with the plugin.

See the `C++ API Docs <api/group__host.html>`_ for complete details.

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.World`
      - `Wrapper <api/classlvtk_1_1World.html>`__
      - N/A

--------
Instance
--------

A LV2 plugin instance.  The :class:`lvtk.World` creates this via it's `instantiate`
method.  It is up to client code what to do with the plugin.

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Instance`
      - `Wrapper <api/classlvtk_1_1Instance.html>`__
      - N/A
    * - :class:`lvtk.PluginInfo`
      - `Struct <api/structlvtk_1_1PluginInfo.html>`__
      - N/A
    * - :class:`lvtk.SupportedUI`
      - `Struct <api/structlvtk_1_1SupportedUI.html>`__
      - N/A
    * - :class:`lvtk.SupportedUIs`
      - `Container <api/classlvtk_1_1SupportedUIs.html>`__
      - N/A

----------
InstanceUI
----------

A LV2 UI instance.  The :class:`lvtk.Instance` creates this via it's `instantiate_ui`
method.  It is up to client code what to do with the UI.

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.InstanceUI`
      - `Wrapper <api/classlvtk_1_1InstanceUI.html>`__
      - N/A
