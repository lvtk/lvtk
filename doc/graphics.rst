########
Graphics
########

Graphics documentation.  Part of the Widgets library. Detailed documenation coming
soon. See the current API docs for more information 
`C++ API documentation <api/group__widgets.html>`_.

-----
Point
-----

Represents an XY coordinate.

---------
Rectangle
---------

Represents a X, Y, Width, Height rectangle where 0,0 is top left. width goes left to right 
and height goes top to bottom.

----
Path
----

Represents a vector path.  Will be used for drawing complex shapes.

---------------
Drawing Context
---------------

Lowlevel drawing context. e.g. OpenGL uses nanovg for drawing.

--------
Graphics
--------

Highlevel drawing context.  Passed to widgets by the View for all rendering.

-----
Fonts
-----

Font support in LVTK is current limited to the built-in font.  The Font class
can be used to change size and style though.

--------
Typeface
--------

:class:`lvtk.Typeface` is the data portion of fonts. Currently is a placeholder in the
API.

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Typeface`
      - `Graphics <../api/classlvtk_1_1Typeface.html>`__
      - N/A

----
Font
----

:class:`lvtk.Font` is the working part of font handling.  Use this glass to configure
font style and pass to :class:`lvtk.Graphics` functions.

**Reference**

.. list-table::
    :widths: auto
    :header-rows: 1
    :align: left

    * - Name
      - C++
      - Lua
    * - :class:`lvtk.Font`
      - `Graphics <../api/classlvtk_1_1Font.html>`__
      - N/A
