[![Piplene](https://gitlab.com/lvtk/lvtk/badges/main/pipeline.svg)](https://gitlab.com/lvtk/lvtk/-/pipelines)
[![CodeFactor](https://www.codefactor.io/repository/github/lvtk/lvtk/badge)](https://www.codefactor.io/repository/github/lvtk/lvtk)
[![REUSE status](https://api.reuse.software/badge/gitlab.com/lvtk/lvtk)](https://api.reuse.software/info/gitlab.com/lvtk/lvtk)

# LVTK3

Minimal C++ library with Lua bindings and console. Write your LV2 plugins and gui's in C++.  The Widgets library can be used in any project, it is not directly tied to LV2 UI.

**Heavy Development:** API's will break from time to time until the core system is ready. Plugin/UI templates are largely unchanged, but could radically change in terms of API.  

## Minimal by Design

LVTK is intended to be used as a subproject by LV2 plugin and UI developers. Docs, Lua Bindings, the Widgets Demo, Lilv wrapper tests, and the `lvtk.lv2` plugins are disabled by default so plugin developers don't have to worry about it.  If packaging, these items will need enabled manually via a meson native/cross file or the command line. see [meson_options.txt](meson_options.txt)

## Widgets

| Widget            | Status  | Purpose              |
|-------------------|---------|----------------------|
| `lvtk.Widget`     | Working | Base for all widgets |
| `lvtk.Button`     | Close   | Base for most button types |
| `lvtk.TextButton` | Close   | A regular button that shows text |
| `lvtk.Label`      | -       | A text display Widget |
| `lvtk.Entry`      | Started | A basic single-line text entry |
| `lvtk.Ranged`     | Working | Base for Widgets with a ralue range.
| `lvtk.Slider`     | Close   | A basic single-value slider control |
| `lvtk.Dial`       | Started | A basic single-value Dial (or Knob) control |
| `lvtk.Embed`      | Started | Embed OS Native Windows/Views |

**Status Legend**

* **Finished** And ready to go.
* **Working** but not finished in terms of fine details and final type names and signatures.
* **Close** to working, functionality partially implemented.
* **Started** some RND, highly experimental.

## Building

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

## Widgets Demo

### Linux & Windows
Depending on build options there are Demos for each backend. From the build directory run:
```
./lvtk-cairo-demo # run the Cairo demo
```
... or ...
```
./lvtk-opengl-demo  # run the OpenGL demo
```

### macOS
Double-clickable Mac apps can be found somewhere in the build directory. The following command will list them all out.
```bash
# From the source top dir.
find build -name "LVTK*.app"
```

## ISC License

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

## 3rd Party Code

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
| Cairo        | Graphics backend| MPL     |

## Issue Tracking

Please report bugs and feature requests on [GitHub](https://github.com/lvtk/lvtk/issues) or [GitLab](https://gitlab.com/lvtk/lvtk/-/issues). 
