LVTK3
=====

Minimal C++ library with Lua bindings and console.
Write your LVplugin gui's in Lua!

Heavy Development
-----------------
This branch is under heavy development.  API's will break from time to time
until the core system is ready.

Built-In Widgets
----------------
- `lvtk.Widget`
- `lvtk.
- to be determined


Building
--------
To build and install, run
```
$ meson setup build
$ meson test -C build
$ ninja install -C build
```

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
The UI classes utilize these libraries internally:
- STB Image - public domain or MIT
- NanoVG - MIT (I think)
- Lua Headers for lua modules - MIT
- Sol2 for c++ lua class bindings - MIT
- `lualv` is a fork of Lua v5.4.4
- boost/signals2 . Observer interfaces.

Issue Tracking
--------------
Please report bugs and feature requests here. 
https://github.com/lvtk/lvtk/issues
