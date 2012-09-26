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
import autowaf, cross, lv2, git

DAPS_VERSION="0.1.0"
DAPS_MAJOR_VERSION=DAPS_VERSION[0]
DAPS_MINOR_VERSION=DAPS_VERSION[2]
DAPS_MICRO_VERSION=DAPS_VERSION[4]

# Anything appended to version number goes here
DAPS_EXTRA_VERSION=""

# For waf dist
APPNAME = 'daps'
VERSION = DAPS_VERSION + DAPS_EXTRA_VERSION

LIB_DAPS       = APPNAME+"-plugin"+DAPS_MAJOR_VERSION
LIB_DAPS_GTKUI = APPNAME+"-gtkui"+DAPS_MAJOR_VERSION

# Required by waf
top = '.'
out = 'build'

def options(opts):
	opts.load("cross compiler_c compiler_cxx lv2")
	autowaf.set_options(opts)
	
	opts.add_option('--disable-tools', default=False, \
		dest="disable_tools", action='store_true', \
		help="Disable Building UI libraries")
	opts.add_option('--disable-ui', default=False, \
		dest="disable_ui", action='store_true', \
		help="Disable Building UI libraries")
	opts.add_option('--ziptype', default='gz', \
		dest='ziptype', type='string', \
		help="Zip type for waf dist (gz/bz2/zip) [ Default: gz ]")


def configure(conf):
	conf.load("cross compiler_c compiler_cxx lv2")

	# Check for required packages
	autowaf.check_pkg(conf, "lv2", uselib_store="lv2", \
				atleast_version="1.0.0")

	if not conf.options.disable_tools:
		autowaf.check_pkg(conf, "redland", uselib_store="redland", \
				atleast_version="1.0.10")
	
	if not conf.options.disable_ui:
		autowaf.check_pkg(conf, "gtkmm-2.4", uselib_store="gtkmm", \
				atleast_version="2.20.0")

	# Setup the Environment
	conf.env.TOOLS_DISABLED	    = conf.options.disable_tools
	conf.env.UI_DISABLED        = conf.options.disable_ui	
	conf.env.DAPS_MAJOR_VERSION = DAPS_MAJOR_VERSION
	conf.env.DAPS_MINOR_VERSION = DAPS_MINOR_VERSION
	conf.env.LIB_DAPS           = LIB_DAPS
	conf.env.LIB_DAPS_GTKUI     = LIB_DAPS_GTKUI
	conf.env.APPNAME	    = APPNAME
	
	autowaf.configure(conf)
	

def build(bld):
	subdirs = ['src','tools','examples']
	for subdir in subdirs: bld.recurse(subdir)
	
	# Build PC Files
	autowaf.build_pc(bld, 'DAPS', DAPS_VERSION, DAPS_MAJOR_VERSION, [],
						{'DAPS_MAJOR_VERSION' : DAPS_MAJOR_VERSION,
						'THELIB'		       : LIB_DAPS,
						'DAPS_PKG_DEPS'       : 'lv2'})
	autowaf.build_pc(bld, 'DAPS-GTKUI', DAPS_VERSION, DAPS_MAJOR_VERSION, [],
						{'DAPS_MAJOR_VERSION' : DAPS_MAJOR_VERSION,
						'VERSION'              : DAPS_VERSION,
						'THELIB'		       : LIB_DAPS_GTKUI,
						'DAPS_PKG_DEPS'       : 'lv2'})
	
	# Install Static Libraries
	bld.install_files(bld.env['LIBDIR'], bld.path.ant_glob("build/**/*.a"))
	
	# Documentation
	autowaf.build_dox(bld, 'DAPS', DAPS_VERSION, top, out)
	
	# Header Installation
	header_base = bld.env['INCLUDEDIR'] + "/" \
				+ APPNAME + "-" + DAPS_MAJOR_VERSION	
	bld.install_files(header_base+"/lv2mm", \
					  bld.path.ant_glob("lv2mm/*.*"))
	bld.install_files(header_base+"/lv2mm/private", \
					  bld.path.ant_glob("lv2mm/private/*.*"))

def release_tag(ctx):
	tag = git.tag_version(VERSION, "Release: v" + VERSION , "v")
	if tag : print "Git Tag Created: " + tag

def dist(ctx):
    z=ctx.options.ziptype
    if 'zip' in z:
        ziptype = z
    else:
        ziptype = "tar." + z
    ctx.algo       = ziptype
