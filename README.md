LV2 Toolkit
===========

This software package contains libraries that wrap the LV2 C API and extensions 
into easy to use C++ classes.

Status
------
[![Build Status](https://travis-ci.org/lvtk/lvtk.svg?branch=master)](https://travis-ci.org/lvtk/lvtk)

Building
--------
To build and install, run
```
$ ./waf configure
$ ./waf build
$ sudo ./waf install
```
To build without the UIs, run

`$ ./waf configure --disable-ui build`

To build documentation, run

`$ ./waf configure --docs build`

HTML documentation gets placed in build/doc/html

Packaging
---------
It is recommended for the 2.0 variety of LVTK to **completely replace** version 1.

However, if v1 and v2 are to be installed on the same system, you will want to
use the `--bundle` option in configure.  This allows you to rename the `lvtk.lv2`
bundle name so there aren't conflicts:
```
./waf configure --bundle=lvtk-2.lv2
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

Issue Tracking 
-------------- 
Please report bugs and feature requests here. 
https://github.com/lvtk/lvtk/issues
