PACKAGE_NAME = lv2-c++-tools
PACKAGE_VERSION = 0.1.363
PKG_DEPS = gtkmm-2.4>=2.8.8

ARCHIVES = liblv2-plugin.a liblv2-gui.a libpaq.a
LIBRARIES = libpaq.so
PROGRAMS = lv2peg

liblv2-plugin_a_SOURCES = lv2plugin.cpp
liblv2-plugin_a_HEADERS = \
	lv2plugin.hpp \
	lv2advanced.hpp \
	lv2synth.hpp \
	../../headers/lv2.h \
	../../headers/lv2-command.h \
	../../headers/lv2-midiport.h \
	../../headers/lv2-midifunctions.h
liblv2-plugin_a_CFLAGS = -Iheaders
liblv2-plugin_a_SOURCEDIR = libraries/lv2plugin
liblv2-plugin_a_INSTALLDIR = $(libdir)

liblv2-gui_a_SOURCES = lv2gui.cpp
liblv2-gui_a_HEADERS = lv2gui.hpp ../../headers/lv2-gui.h
liblv2-gui_a_CFLAGS = `pkg-config --cflags gtkmm-2.4` -Iheaders
liblv2-gui_a_SOURCEDIR = libraries/lv2gui
liblv2-gui_a_INSTALLDIR = $(libdir)

libpaq_a_SOURCES = turtleparser.cpp rdf.cpp query.cpp
libpaq_a_SOURCEDIR = libraries/paq
libpaq_a_INSTALLDIR = $(libdir)

libpaq_so_ABI = 0
libpaq_so_ARCHIVES = libraries/paq/libpaq.a
libpaq_so_HEADERS = turtleparser.hpp rdf.hpp query.hpp unicode.hpp namespaces.hpp
libpaq_so_SOURCEDIR = libraries/paq

lv2peg_SOURCES = lv2peg.cpp
lv2peg_CFLAGS = -Ilibraries/paq -DVERSION=\"$(PACKAGE_VERSION)\"
lv2peg_LIBRARIES = libraries/paq/libpaq.so.0.0.0
lv2peg_SOURCEDIR = programs/lv2peg

DOCS = COPYING AUTHORS README
PCFILES = lv2-plugin.pc lv2-gui.pc paq.pc


# Do the magic
include Makefile.template

