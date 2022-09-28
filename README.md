![Piplene](https://gitlab.com/lvtk/lvtk/badges/main/pipeline.svg)

LVTK3
=====

Minimal C++ library with Lua bindings and console. Write your LV2 plugins and gui's in C++ or Lua!

Heavy Development
-----------------
API's will break from time to time until the core system is ready.  Plugin/UI templates are largely unchanged.  
It's the widgets and host libraries which will see the most refactoring

Built-In Widgets
----------------
| Widget            | Purpose              |
|-------------------|----------------------|
| `lvtk.Widget`     | Base for all widgets |
| `lvtk.Button`     | Base for most button types |
| `lvtk.TextButton` | A regular button that shows text |
| `lvtk.Entry`      | A basic single-line text entry |
| `lvtk.Slider`     | A basic single-value slider control |
| `lvtk.Dial`       | A basic single-value Dial (or Knob) control |

Building
--------
To build and install, run
```
$ meson setup build
$ ninja -C build
$ ninja -C test
$ ninja install # optional
```

For MSVC on Windows, use:
```
meson setup build
meson compile -C build
meson test -C build
```

Gui Demo
--------
```
cd build
./lvtk-demo
```
... launches the C++ demo, and ...  
```
bash scripts/demo.sh
```
... should run the Lua gui demo loaded by `lvtk.import`

ISC License
-----------
```
Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```

3rd Party Code
--------------
Graphics/UI/Hosting classes utilize these libraries internally:

| Library      | For?            | License |
|--------------|-----------------|---------|
| STB Image    | Images          | MIT     |
| STB Truetype | Font Loading    | MIT     |
| NanoVG       | OpenGL Graphics | Zlib    |
| Lua          | Lua Bindings    | MIT     |
| Sol2         | Lua Bindings    | MIT     |
| Boost        | Tests/Signals   | Boost   |
| Lilv         | LV2 Hosting     | ISC     |
| Suil         | LV2UI Hosting   | ISC     |

Issue Tracking
--------------
Please report bugs and feature requests here. 
https://gitlab.com/lvtk/lvtk/-/issues
