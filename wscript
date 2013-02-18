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

LVTK_VERSION="1.0.4"
LVTK_MAJOR_VERSION=LVTK_VERSION[0]
LVTK_MINOR_VERSION=LVTK_VERSION[2]
LVTK_MICRO_VERSION=LVTK_VERSION[4]

# Anything appended to version number goes here
LVTK_EXTRA_VERSION=""

# For waf dist
APPNAME = 'lvtk'
VERSION = LVTK_VERSION + LVTK_EXTRA_VERSION

LIB_LVTK_PLUGIN      = APPNAME + "_plugin" + LVTK_MAJOR_VERSION
LIB_LVTK_UI      	 = APPNAME + "_ui" + LVTK_MAJOR_VERSION

# Required by waf
top = '.'
out = 'build'

def options(opts):
	opts.load("cross compiler_c compiler_cxx lv2 boost")
	autowaf.set_options(opts)
	
	opts.add_option('--disable-tools', default=False, \
		dest="disable_tools", action='store_true', \
		help="Disable Building Tools")
	opts.add_option('--disable-ui', default=False, \
		dest="disable_ui", action='store_true', \
		help="Disable Building UI libraries")
	opts.add_option('--disable-examples', default=False, \
		dest="disable_examples", action='store_true', \
		help="Disable Building Examples")
	opts.add_option('--ziptype', default='gz', \
		dest='ziptype', type='string', \
		help="Zip type for waf dist (gz/bz2/zip) [ Default: gz ]")


def configure(conf):
	conf.load("cross compiler_c compiler_cxx lv2 boost")

	a = autowaf
	
	conf.define ("LVTK_VERSION", VERSION)
	conf.define ("LVTK_MAJOR_VERSION",LVTK_MAJOR_VERSION)
	conf.define ("LVTK_MINOR_VERSION",LVTK_MINOR_VERSION)
	conf.define ("LVTK_MICRO_VERSION",LVTK_MICRO_VERSION)
	conf.define ("LVTK_EXTRA_VERSION",LVTK_EXTRA_VERSION)
	conf.write_config_header("version.h")

	conf.check_inline()

	if not conf.options.disable_tools: conf.check_boost()
		
	# Check for required packages
	conf.check_lv2 ("1.2.0")
	
	# UI Configuration
	autowaf.check_pkg(conf, "gtkmm-2.4", uselib_store="gtkmm", \
				atleast_version="2.20.0", mandatory=False)
	
	# Setup the Environment
	conf.env.EXAMPLES_DISABLED  = conf.options.disable_examples
	conf.env.TOOLS_DISABLED	    = conf.options.disable_tools
	conf.env.UI_DISABLED        = conf.options.disable_ui
	
	# Examples plugins depend on tools
	if conf.env.TOOLS_DISABLED or conf.env.EXAMPLES_DISABLED:
		conf.env.BUILD_EXAMPLE_PLUGINS = False
	else: conf.env.BUILD_EXAMPLE_PLUGINS = True
	
	# Example UI's depend on Gtkmm and Plugins
	if conf.env.LIB_gtkmm and conf.env.BUILD_EXAMPLE_PLUGINS and not conf.env.EXAMPLES_DISABLED:
		conf.env.BUILD_EXAMPLE_UIS  = True
	else: conf.env.BUILD_EXAMPLE_UIS  = False
			
	conf.env.LVTK_MAJOR_VERSION = LVTK_MAJOR_VERSION
	conf.env.LVTK_MINOR_VERSION = LVTK_MINOR_VERSION
	conf.env.LIB_LVTK_PLUGIN    = LIB_LVTK_PLUGIN
	conf.env.LIB_LVTK_UI        = LIB_LVTK_UI
	conf.env.APPNAME	        = APPNAME
	
	autowaf.configure(conf)
	
	autowaf.display_header( "LV2 Toolkit Configuration")
	autowaf.display_msg(conf,"Build Plugin Library", True)
	autowaf.display_msg(conf,"Build UI Library",True)
	autowaf.display_msg(conf,"Build example plugins", not conf.env.EXAMPLES_DISABLED)
	autowaf.display_msg(conf,"Build example UI's", conf.env.BUILD_EXAMPLE_UIS)
	autowaf.display_msg(conf,"Build tools", not conf.env.TOOLS_DISABLED)
	
def build(bld):
    
	subdirs = ['src','tools','examples']
	for subdir in subdirs: 
		bld.recurse(subdir)
		bld.add_group()
	
	pcvers = LVTK_MAJOR_VERSION
	
	# Build PC Files
	autowaf.build_pc(bld, 'LVTK-PLUGIN', LVTK_VERSION, pcvers, [],
						{'LVTK_MAJOR_VERSION' : LVTK_MAJOR_VERSION,
						'VERSION'              : LVTK_VERSION,
						'THELIB'		       : LIB_LVTK_PLUGIN,
						'LVTK_PKG_DEPS'       : 'lv2'})
						
	if not bld.env.UI_DISABLED:
		autowaf.build_pc(bld, 'LVTK-UI', LVTK_VERSION, pcvers, [],
						{'LVTK_MAJOR_VERSION' : LVTK_MAJOR_VERSION,
						'VERSION'              : LVTK_VERSION,
						'THELIB'		       : LIB_LVTK_UI,
						'LVTK_PKG_DEPS'       : 'lv2'})
		if bld.env.LIB_gtkmm:
			autowaf.build_pc(bld, 'LVTK-GTKUI', LVTK_VERSION, pcvers, [],
						{'LVTK_MAJOR_VERSION' : LVTK_MAJOR_VERSION,
						'VERSION'              : LVTK_VERSION,
						'THELIB'		       : LIB_LVTK_UI,
						'LVTK_PKG_DEPS'       : 'lv2 gtkmm-2.4'})
						
	bld.add_group()	
	
	# Install Static Libraries
	bld.install_files(bld.env['LIBDIR'], bld.path.ant_glob("build/**/*.a"))
	
	# Documentation
	autowaf.build_dox(bld, 'LVTK', VERSION, top, out)
	bld.add_group()
	
	# Header Installation
	header_base = bld.env['INCLUDEDIR'] + "/" \
				+ APPNAME + "-" + pcvers
				
	bld.install_files(header_base+"/lvtk", "build/version.h")
	
	bld.install_files(header_base+"/lvtk", \
					  bld.path.ant_glob("lvtk/*.*"))
	
	bld.install_files(header_base+"/lvtk/behaviors", \
					  bld.path.ant_glob("lvtk/behaviors/*.*"))
					  
	bld.install_files(header_base+"/lvtk/ext", \
					  bld.path.ant_glob("lvtk/ext/*.*"))
					  
	bld.install_files(header_base+"/lvtk/private", \
					  bld.path.ant_glob("lvtk/private/*.*"))


def release_tag(ctx):
	tag = git.tag_version(VERSION, "Release: v" + VERSION , "lvtk-")
	if tag : print "Git Tag Created: " + tag

def dist(ctx):
    z=ctx.options.ziptype
    if 'zip' in z:
        ziptype = z
    else:
        ziptype = "tar." + z
    ctx.algo       = ziptype
