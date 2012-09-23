#!/usr/bin/env python
# Copyright (C) 2012  Michael Fisher <mfisher31@gmail.com>

''' 
This tool allows you to specify a cross compiler and other tools
via a '--cross' option. An environment variable, CROSS_COMPILE, 
also gets set automatically when valid cross compilers are found.

The wscript will look like:

def options(opt):
    opt.load("cross compiler_c compiler_cxx")
    
def configure(conf):
    opt.load("cross compiler_c compiler_cxx")

In the load method, 'cross must come before the compilers.

Example Usage: 
Cross compile for ARM where the c compiler is arm-linux-gnueabi-gcc
$ ./waf configure --cross=arm-linux-gnueabi
$ ./waf build

If you're lucky, then your binary has been built targeting armel

Note there is not a trailing "-" on gnueabi.  Assuming the appropriate 
compilers are installed on the host system, this will work for ANY cross 
compiling prefix. '''


### Cross Implementation

def options(opt):
    opt.add_option('--cross', default='', dest='cross', type='string', \
        help="The Cross Compiler to Use. e.g. arm-linux-gnueabi [ Default: none ]" )
        
def configure(conf):
    if conf.options.cross:
        # TODO: Define more utils here
        conf.env['CC'] = conf.options.cross+"-gcc"
        conf.env['CXX'] = conf.options.cross+"-g++"
        conf.env['AR'] = conf.options.cross+"-ar"
        conf.env['RANLIB'] = conf.options.cross+"-ranlib"
        conf.env['PKGCONFIG'] = conf.options.cross+"-pkg-config"
        conf.env['CROSS_COMPILE'] = 1
