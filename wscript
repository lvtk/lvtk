#!/usr/bin/env python
# encoding: utf-8
# Copyright (C) 2012 Michael Fisher <mfisher31@gmail.com>

''' 
Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

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
'''

import sys, os
from subprocess import call
from waflib.extras import autowaf as autowaf
sys.path.insert(0, "tools/waf")
import cross, git

LVTK_VERSION='2.0.0'
LVTK_MAJOR_VERSION=LVTK_VERSION[0]
LVTK_MINOR_VERSION=LVTK_VERSION[2]
LVTK_MICRO_VERSION=LVTK_VERSION[4]
LVTK_EXTRA_VERSION=''

# For waf dist
APPNAME = 'lvtk'
VERSION = LVTK_VERSION + LVTK_EXTRA_VERSION

# Required by waf
top = '.'
out = 'build'

def options (opts):
    opts.load("cross compiler_c compiler_cxx lv2 autowaf")
    autowaf.set_options (opts)
    
    opts.add_option('--enable-tests', default=False, \
        dest="tests", action='store_true', help='Build the test suite')
    opts.add_option('--disable-ui', default=False, \
        dest="disable_ui", action='store_true', help='Disable Building UI libraries')
    opts.add_option('--disable-examples', default=False, \
        dest="disable_examples", action='store_true', help='Disable Building Examples')
    opts.add_option('--ziptype', default='gz', \
        dest='ziptype', type='string', help='Zip type for waf dist (gz/bz2/zip) [ Default: gz ]')

def configure (conf):
    conf.load ("cross compiler_c compiler_cxx lv2 autowaf")
    conf.find_program ('ttl2c', mandatory=False)

    conf.define ("LVTK_VERSION", VERSION)
    conf.define ("LVTK_MAJOR_VERSION", LVTK_MAJOR_VERSION)
    conf.define ("LVTK_MINOR_VERSION", LVTK_MINOR_VERSION)
    conf.define ("LVTK_MICRO_VERSION", LVTK_MICRO_VERSION)
    conf.define ("LVTK_EXTRA_VERSION", LVTK_EXTRA_VERSION)
    conf.write_config_header ('version.h')

    autowaf.set_cxx_lang (conf, 'c++14')
    autowaf.set_modern_cxx_flags (conf, True)

    conf.check_inline()
    autowaf.check_pkg (conf, 'lv2', uselib_store='LV2', mandatory=True)
    autowaf.check_pkg (conf, "gtkmm-2.4", uselib_store="GTKMM", atleast_version="2.20.0", mandatory=False)
    autowaf.check_pkg (conf, 'cppunit', uselib_store='CPPUNIT', atleast_version='1.13.0', mandatory=conf.options.tests)
    
    for module in 'audio_basics gui_basics'.split():
        pkgname = 'juce_%s-5' % module if not conf.options.debug else 'juce_%s_debug-5' % module
        uselib  = 'JUCE_%s' % module.upper()
        autowaf.check_pkg (conf, pkgname, uselib_store=uselib, atleast_version="5.4.3", mandatory=False)
    
    # Setup the Environment
    conf.env.TESTS              = conf.options.tests
    conf.env.EXAMPLES_DISABLED  = conf.options.disable_examples
    conf.env.UI_DISABLED        = conf.options.disable_ui
    conf.env.BUILD_EXAMPLE_UIS  = not conf.env.EXAMPLES_DISABLED and not conf.env.UI_DISABLED

    conf.env.LVTK_MAJOR_VERSION = LVTK_MAJOR_VERSION
    conf.env.LVTK_MINOR_VERSION = LVTK_MINOR_VERSION
    conf.env.APPNAME            = APPNAME

    print
    autowaf.display_summary (conf, {
        "LVTK Version": LVTK_VERSION,
        "Build Examples": not conf.env.EXAMPLES_DISABLED,
        "Build Example UIs": conf.env.BUILD_EXAMPLE_UIS
    })

def build (bld):
    if not bld.env.EXAMPLES_DISABLED:
        for subdir in ['examples']:
            bld.recurse (subdir)
            bld.add_group()
    
    bld (
        features    = 'subst',
        source      = 'lvtk.lv2/manifest.ttl',
        target      = 'lvtk.lv2/manifest.ttl',
        install_path = os.path.join (bld.env.LV2DIR, 'lvtk.lv2')
    )

    # Build PC Files
    pcvers = LVTK_MAJOR_VERSION
    autowaf.build_pc (bld, 'LVTK', LVTK_VERSION, pcvers, [],
        { 'LVTK_MAJOR_VERSION'   : LVTK_MAJOR_VERSION,
          'VERSION'              : LVTK_VERSION,
          'LVTK_PKG_DEPS'        : 'lv2'})

    bld.add_group()

    # Install Static Libraries
    bld.install_files(bld.env['LIBDIR'], bld.path.ant_glob("build/**/*.a"))

    # Documentation
    autowaf.build_dox (bld, 'LVTK', VERSION, top, out)
    bld.add_group()
    
    # Tests
    if bld.env.TESTS:
        bld.program (
            features = 'cxx cxxprogram',
            source   = bld.path.ant_glob ('tests/**/*.cpp'),
            name     = 'testlvtk',
            target   = 'testlvtk',
            use      = [ 'CPPUNIT' ],
            cxxflags = ["-DLVTK_NO_SYMBOL_EXPORT"],
            install_path = None
        )

    # Header Installation
    header_base = bld.env.INCLUDEDIR + "/"  + APPNAME + "-" + pcvers
    bld.install_files(header_base+"/lvtk", "build/version.h")
    bld.install_files(header_base+"/lvtk", bld.path.ant_glob("lvtk/*.*"))
    bld.install_files(header_base+"/lvtk/ext", bld.path.ant_glob("lvtk/ext/*.*"))
    bld.install_files(header_base+"/lvtk/host", bld.path.ant_glob("lvtk/host/*.*"))
    bld.install_files(header_base+"/lvtk/ext", bld.path.ant_glob("lvtk/ext/*.*"))

def check(ctx):
    call('build/testlvtk')

def dist(ctx):
    z=ctx.options.ziptype
    if 'zip' in z:
        ziptype = z
    else:
        ziptype = "tar." + z
    ctx.algo       = ziptype
