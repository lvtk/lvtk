PACKAGE_NAME = lv2-c++-tools
PACKAGE_VERSION = 0.1.330
PKG_DEPS = gtkmm-2.4>=2.8.8

ARCHIVES = liblv2_plugin.a liblv2_gtk2gui.a libpaq.a
PROGRAMS = lv2peg

liblv2_plugin_a_SOURCES = lv2plugin.cpp
liblv2_plugin_a_HEADERS = \
	lv2plugin.hpp \
	lv2advanced.hpp \
	../../headers/lv2.h \
	../../headers/lv2-command.h \
	../../headers/lv2-midiport.h \
	../../headers/lv2-midifunctions.h
liblv2_plugin_a_CFLAGS = -Iheaders
liblv2_plugin_a_SOURCEDIR = libraries/lv2plugin
liblv2_plugin_a_INSTALLDIR = $(libdir)

liblv2_gtk2gui_a_SOURCES = lv2gtk2gui.cpp
liblv2_gtk2gui_a_HEADERS = lv2gtk2gui.hpp ../../headers/lv2-gui.h
liblv2_gtk2gui_a_CFLAGS = `pkg-config --cflags gtkmm-2.4` -Iheaders
liblv2_gtk2gui_a_SOURCEDIR = libraries/lv2gtk2gui
liblv2_gtk2gui_a_INSTALLDIR = $(libdir)

libpaq_a_SOURCES = turtleparser.cpp rdf.cpp query.cpp
libpaq_a_HEADERS = turtleparser.hpp rdf.hpp query.hpp unicode.hpp namespaces.hpp
libpaq_a_SOURCEDIR = libraries/paq

lv2peg_SOURCES = lv2peg.cpp
lv2peg_CFLAGS = -Ilibraries/paq -DVERSION=\"$(PACKAGE_VERSION)\"
lv2peg_ARCHIVES = libraries/paq/libpaq.a
lv2peg_SOURCEDIR = programs/lv2peg

DOCS = COPYING AUTHORS README


# Do the magic
include Makefile.template
