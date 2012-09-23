#!/usr/bin/evn python
# encoding: utf-8
# Copyright (C) 2012 Michael Fisher <mfisher31@gmail.com>

''' This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public Licence as published by
the Free Software Foundation, either version 3 of the Licence, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
file COPYING for more details. '''

import sys
from subprocess import call

sys.path.insert(0, "tools/waf")
import autowaf, cross, git

LV2MM_VERSION="0.0.1"
LV2MM_MAJOR_VERSION=LV2MM_VERSION[0]
LV2MM_MINOR_VERSION=LV2MM_VERSION[2]
LV2MM_MICRO_VERSION=LV2MM_VERSION[4]

# Anything appended to version number goes here
LV2MM_EXTRA_VERSION="-rc1"

# For waf dist
APPNAME = 'lv2mm'
VERSION = LV2MM_VERSION + LV2MM_EXTRA_VERSION

LIB_LV2MM       = APPNAME+"-plugin"+LV2MM_MAJOR_VERSION
LIB_LV2MM_GTKUI = APPNAME+"-gtkui"+LV2MM_MAJOR_VERSION

# Required by waf
top = '.'
out = 'build'

def options(opts):
	opts.load("cross compiler_c compiler_cxx")
	autowaf.set_options(opts)
	
	opts.add_option('--disable-ui', default=False, \
		dest="disable_ui", action='store_true', \
		help="Disable Building UI libraries")
	opts.add_option('--ziptype', default='gz', \
		dest='ziptype', type='string', \
		help="Zip type for waf dist (gz/bz2/zip) [ Default: gz ]")


def configure(conf):
	conf.load("cross compiler_c compiler_cxx")

	# Check for required packages
	autowaf.check_pkg(conf, "lv2", uselib_store="lv2", \
				atleast_version="1.0.0")
	autowaf.check_pkg(conf, "redland", uselib_store="redland", \
				atleast_version="1.0.10")
	autowaf.check_pkg(conf, "gtkmm-2.4", uselib_store="gtkmm", \
				atleast_version="2.20.0")

	# Setup the Environment
	conf.env.UI_DISABLED         = conf.options.disable_ui	
	conf.env.LV2MM_MAJOR_VERSION = LV2MM_MAJOR_VERSION
	conf.env.LV2MM_MINOR_VERSION = LV2MM_MINOR_VERSION
	conf.env.LIB_LV2MM           = LIB_LV2MM
	conf.env.LIB_LV2MM_GTKUI     = LIB_LV2MM_GTKUI
	conf.env.APPNAME			 = APPNAME
	
	autowaf.configure(conf)
	

def build(bld):
	subdirs = ['src','tools']
	for subdir in subdirs: bld.recurse(subdir)
	
	# Build PC Files
	autowaf.build_pc(bld, 'LV2MM', LV2MM_VERSION, LV2MM_MAJOR_VERSION, [],
						{'LV2MM_MAJOR_VERSION' : LV2MM_MAJOR_VERSION,
						'THELIB'		       : LIB_LV2MM,
						'LV2MM_PKG_DEPS'       : 'lv2'})
	autowaf.build_pc(bld, 'LV2MM-GTKUI', LV2MM_VERSION, LV2MM_MAJOR_VERSION, [],
						{'LV2MM_MAJOR_VERSION' : LV2MM_MAJOR_VERSION,
						'VERSION'              : LV2MM_VERSION,
						'THELIB'		       : LIB_LV2MM_GTKUI,
						'LV2MM_PKG_DEPS'       : 'lv2'})
	
	# Install Static Libraries
	bld.install_files(bld.env['LIBDIR'], bld.path.ant_glob("build/**/*.a"))
	
	# Documentation
	autowaf.build_dox(bld, 'LV2MM', LV2MM_VERSION, top, out)
	
	# Header Installation
	header_base = bld.env['INCLUDEDIR'] + "/" \
				+ APPNAME + "-" + LV2MM_MAJOR_VERSION	
	bld.install_files(header_base+"/lv2mm", \
					  bld.path.ant_glob("lv2mm/*.*"))
	bld.install_files(header_base+"/lv2mm/private", \
					  bld.path.ant_glob("lv2mm/private/*.*"))

def release_tag(ctx):
	git.tag_version(VERSION, "Release: " + APPNAME + "-" + VERSION)

def dist(ctx):
    z=ctx.options.ziptype
    if 'zip' in z:
        ziptype = z
    else:
        ziptype = "tar." + z
    ctx.algo       = ziptype