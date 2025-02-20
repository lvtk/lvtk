[![Build](https://github.com/lvtk/lvtk/actions/workflows/meson.yml/badge.svg)](https://github.com/lvtk/lvtk/actions)
[![CodeFactor](https://www.codefactor.io/repository/github/lvtk/lvtk/badge)](https://www.codefactor.io/repository/github/lvtk/lvtk)
[![REUSE status](https://api.reuse.software/badge/github.com/lvtk/lvtk)](https://api.reuse.software/info/github.com/lvtk/lvtk)

# LVTK

LVTK is a minimal C++ library with Lua bindings and console. Write your LV2 plugins and gui's in C++. It is a header-only library and intended to be used as a subproject by LV2 plugin and UI developers.  This being the case, pkg-config files and typical installation is available.

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

## Testing
The tests depend on boost
```
sudo apt install libboost-test
```
... or ...
```
BOOST_ROOT="/path/to/somewhere" meson setup build
```

Then simply run meson test
```
meson test -C build
```

## Meson Subproject
See the guide on [using a subproject](https://mesonbuild.com/Subprojects.html#using-a-subproject) on [mesonbuild.com](https://mesonbuild.com)

## The Docs
In order to build the documentation you will need:
```bash
sudo apt install graphviz doxygen python3-sphinx python3-sphinx-rtd-theme
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

## Issue Tracking

Please report bugs and feature requests on [GitHub](https://github.com/lvtk/lvtk/issues) or [GitLab](https://gitlab.com/lvtk/lvtk/-/issues).
