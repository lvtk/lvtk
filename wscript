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

LVTK_VERSION="1.0.0"
LVTK_MAJOR_VERSION=LVTK_VERSION[0]
LVTK_MINOR_VERSION=LVTK_VERSION[2]
LVTK_MICRO_VERSION=LVTK_VERSION[4]

# Anything appended to version number goes here
LVTK_EXTRA_VERSION=""

# For waf dist
APPNAME = 'lvtk'
VERSION = LVTK_VERSION + LVTK_EXTRA_VERSION

LIB_LVTK        = APPNAME + LVTK_MAJOR_VERSION
LIB_LVTKUI      = APPNAME + "ui" + LVTK_MAJOR_VERSION

# Required by waf
top = '.'
out = 'build'

def options(opts):
	opts.load("cross compiler_c compiler_cxx lv2")
	autowaf.set_options(opts)
	
	opts.add_option('--disable-tools', default=False, \
		dest="disable_tools", action='store_true', \
		help="Disable Building Tools")
	opts.add_option('--disable-ui', default=False, \
		dest="disable_ui", action='store_true', \
		help="Disable Building UI libraries")
	opts.add_option('--ziptype', default='gz', \
		dest='ziptype', type='string', \
		help="Zip type for waf dist (gz/bz2/zip) [ Default: gz ]")


def configure(conf):
	conf.load("cross compiler_c compiler_cxx lv2")

	conf.define ("LVTK_VERSION", VERSION)
	conf.define ("LVTK_MAJOR_VERSION",LVTK_MAJOR_VERSION)
	conf.define ("LVTK_MINOR_VERSION",LVTK_MINOR_VERSION)
	conf.define ("LVTK_MICRO_VERSION",LVTK_MICRO_VERSION)
	conf.define ("LVTK_EXTRA_VERSION",LVTK_EXTRA_VERSION)
	conf.write_config_header("version.hpp")

	# Check for required packages
	autowaf.check_pkg(conf, "lv2", uselib_store="lv2", \
				atleast_version="1.0.0")

	if not conf.options.disable_tools:
		conf.check(header_name='boost/spirit/core.hpp')
		conf.check(header_name='boost/spirit/utility.hpp')
		conf.check(header_name='boost/spirit/tree/parse_tree.hpp')
		conf.check(header_name='boost/spirit/tree/ast.hpp')
		
	if not conf.options.disable_ui:
		autowaf.check_pkg(conf, "gtkmm-2.4", uselib_store="gtkmm", \
				atleast_version="2.20.0")

	# Setup the Environment
	conf.env.TOOLS_DISABLED	    = conf.options.disable_tools
	conf.env.UI_DISABLED        = conf.options.disable_ui	
	conf.env.LVTK_MAJOR_VERSION = LVTK_MAJOR_VERSION
	conf.env.LVTK_MINOR_VERSION = LVTK_MINOR_VERSION
	conf.env.LIB_LVTK           = LIB_LVTK
	conf.env.LIB_LVTKUI         = LIB_LVTKUI
	conf.env.APPNAME	        = APPNAME
	
	autowaf.configure(conf)
	

def build(bld):
    
	subdirs = ['src','tools','examples']
	for subdir in subdirs: 
		bld.recurse(subdir)
		bld.add_group()
	
	pcvers = LVTK_MAJOR_VERSION + "." + LVTK_MINOR_VERSION
	
	# Build PC Files
	autowaf.build_pc(bld, 'LVTK', LVTK_VERSION, pcvers, [],
						{'LVTK_MAJOR_VERSION' : LVTK_MAJOR_VERSION,
						'VERSION'              : LVTK_VERSION,
						'THELIB'		       : LIB_LVTK,
						'LVTK_PKG_DEPS'       : 'lv2'})
						
	if not bld.env.UI_DISABLED:
		autowaf.build_pc(bld, 'LVTKUI', LVTK_VERSION, pcvers, [],
						{'LVTK_MAJOR_VERSION' : LVTK_MAJOR_VERSION,
						'VERSION'              : LVTK_VERSION,
						'THELIB'		       : LIB_LVTKUI,
						'LVTK_PKG_DEPS'       : 'lv2'})
	bld.add_group()	
	
	# Install Static Libraries
	bld.install_files(bld.env['LIBDIR'], bld.path.ant_glob("build/**/*.a"))
	
	# Documentation
	autowaf.build_dox(bld, 'LVTK', VERSION, top, out)
	bld.add_group()
	
	# Header Installation
	header_base = bld.env['INCLUDEDIR'] + "/" \
				+ APPNAME + "-" + pcvers
	bld.install_files(header_base, "build/version.hpp")
	bld.install_files(header_base+"/lvtk", \
					  bld.path.ant_glob("lvtk/*.*"))
	bld.install_files(header_base+"/lvtk/private", \
					  bld.path.ant_glob("lvtk/private/*.*"))


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
