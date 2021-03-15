#!/usr/bin/env python
# encoding: utf-8
# Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

'''
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

LVTK_VERSION='2.0.0'
LVTK_MAJOR_VERSION=LVTK_VERSION[0]
LVTK_MINOR_VERSION=LVTK_VERSION[2]
LVTK_MICRO_VERSION=LVTK_VERSION[4]
LVTK_EXTRA_VERSION='rc1'

# For waf dist
APPNAME = 'lvtk'
VERSION = LVTK_VERSION + LVTK_EXTRA_VERSION

# Required by waf
top = '.'
out = 'build'

def options (opts):
    opts.load("compiler_c compiler_cxx lv2 autowaf")
    autowaf.set_options (opts)
    
    opts.add_option('--bundle', default='lvtk.lv2', \
        dest='bundle', type='string', help='Bundle name [ Default: lvtk.lv2 ]')
    opts.add_option('--tests', default=False, \
        dest="tests", action='store_true', help='Build the test suite')
    opts.add_option('--disable-ui', default=False, \
        dest="disable_ui", action='store_true', help='Disable Building UI libraries')
    opts.add_option('--disable-examples', default=False, \
        dest="disable_examples", action='store_true', help='Disable Building Examples')
    opts.add_option('--ziptype', default='gz', \
        dest='ziptype', type='string', help='Zip type for waf dist (gz/bz2/zip) [ Default: gz ]')

def configure (conf):
    conf.load ("compiler_c compiler_cxx lv2 autowaf")

    conf.define ("LVTK_VERSION", VERSION)
    conf.define ("LVTK_MAJOR_VERSION", LVTK_MAJOR_VERSION)
    conf.define ("LVTK_MINOR_VERSION", LVTK_MINOR_VERSION)
    conf.define ("LVTK_MICRO_VERSION", LVTK_MICRO_VERSION)
    conf.define ("LVTK_EXTRA_VERSION", LVTK_EXTRA_VERSION)
    conf.write_config_header ('version.h')

    autowaf.set_modern_cxx_flags (conf, True)
    conf.env.append_unique ('CFLAGS', ['-fvisibility=hidden'])
    conf.env.append_unique ('CXXFLAGS', ['-fvisibility=hidden'])

    conf.check_inline()
    autowaf.check_pkg (conf, 'lv2', uselib_store='LV2', mandatory=True)
    autowaf.check_pkg (conf, 'cppunit', uselib_store='CPPUNIT', atleast_version='1.13.0', mandatory=conf.options.tests)
      
    # Setup the Environment
    conf.env.BUNDLE             = conf.options.bundle
    if len(conf.env.BUNDLE) <= 0: 
        conf.env.BUNDLE = 'lvtk.lv2'
    conf.env.TESTS              = conf.options.tests
    conf.env.EXAMPLES_DISABLED  = conf.options.disable_examples
    conf.env.UI_DISABLED        = conf.options.disable_ui
    conf.env.BUILD_EXAMPLE_UIS  = not conf.env.EXAMPLES_DISABLED and not conf.env.UI_DISABLED

    conf.env.LVTK_MAJOR_VERSION = LVTK_MAJOR_VERSION
    conf.env.LVTK_MINOR_VERSION = LVTK_MINOR_VERSION
    conf.env.APPNAME            = APPNAME

    print
    autowaf.display_summary (conf, {
        "LVTK Version": VERSION,
        "LVTK Bundle": conf.env.BUNDLE,
        "Build Examples": not conf.env.EXAMPLES_DISABLED,
        "Build Example UIs": conf.env.BUILD_EXAMPLE_UIS
    })

def build (bld):
    if not bld.env.EXAMPLES_DISABLED:
        for subdir in ['plugins']:
            bld.recurse (subdir)
            bld.add_group()

    # Build PC File
    pcvers = LVTK_MAJOR_VERSION
    autowaf.build_pc (bld, 'LVTK', VERSION, pcvers, [],
        { 'LVTK_MAJOR_VERSION'   : LVTK_MAJOR_VERSION,
          'VERSION'              : VERSION,
          'LVTK_PKG_DEPS'        : 'lv2'})

    bld.add_group()

    # Documentation
    autowaf.build_dox (bld, 'LVTK', VERSION, top, out)
    bld.add_group()
    
    # Tests
    if bld.env.TESTS:
        bld.program (
            features = 'cxx cxxprogram',
            source   = bld.path.ant_glob ('tests/**/*.cpp') + \
                       [ 'plugins/volume.cpp' ],
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
    bld.install_files(header_base+"/lvtk/ext/ui", bld.path.ant_glob("lvtk/ext/ui/*.*"))

def check (ctx):
    if not os.path.exists ('build/testlvtk'):
        ctx.fatal ("Tests were not compiled")
        return
    if 0 != call ('build/testlvtk'):
        ctx.fatal ("Tests Failed")

def dist(ctx):
    z=ctx.options.ziptype
    if 'zip' in z:
        ziptype = z
    else:
        ziptype = "tar." + z
    ctx.algo       = ziptype
    ctx.base_name = '%s-%s' % (APPNAME, VERSION)
    ctx.excl =  ' **/.waf-1* **/.waf-2* **/.waf-3 **/waf-1* **/waf-2* **/waf-3*'
    ctx.excl += ' **/*~ **/*.pyc **/*.swp **/.lock-w*'
    ctx.excl += ' **/.gitignore **/.gitmodules **/.git dist build **/.DS_Store'
    ctx.excl += ' **/.vscode **/.travis.yml'
