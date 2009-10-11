PACKAGE_NAME = lv2-c++-tools
PACKAGE_VERSION = 1.0.2
PKG_DEPS = \
	gtkmm-2.4>=2.8.8 \
	redland>=1.0.9

ARCHIVES = liblv2-plugin.a liblv2-gui.a
PROGRAMS = lv2peg
DATAPACKS = lv2soname

# The static plugin library with headers
liblv2-plugin_a_SOURCES = lv2plugin.cpp
liblv2-plugin_a_HEADERS = \
	lv2plugin.hpp \
	lv2synth.hpp \
	../../headers/lv2.h \
	../../headers/lv2_event.h \
	../../headers/lv2_event_helpers.h \
	../../headers/lv2_osc.h \
	../../headers/lv2_uri_map.h \
	../../headers/lv2_saverestore.h \
	../../headers/lv2_contexts.h \
	../../headers/lv2types.hpp
liblv2-plugin_a_CFLAGS = -Iheaders -Ilibraries/lv2plugin
liblv2-plugin_a_SOURCEDIR = libraries/lv2plugin
liblv2-plugin_a_INSTALLDIR = $(libdir)

# The static GUI library with headers
liblv2-gui_a_SOURCES = lv2gui.cpp
liblv2-gui_a_HEADERS = \
	lv2gui.hpp \
	../../headers/lv2_ui.h \
	../../headers/lv2_ui_presets.h
liblv2-gui_a_CFLAGS = `pkg-config --cflags gtkmm-2.4` -Iheaders
liblv2-gui_a_SOURCEDIR = libraries/lv2gui
liblv2-gui_a_INSTALLDIR = $(libdir)

# lv2peg
lv2peg_SOURCES = lv2peg.cpp
lv2peg_CFLAGS = -DVERSION=\"$(PACKAGE_VERSION)\" `pkg-config --cflags redland`
lv2peg_LDFLAGS = `pkg-config --libs redland`
lv2peg_SOURCEDIR = programs/lv2peg

# lv2soname
lv2soname_FILES = lv2soname
lv2soname_SOURCEDIR = programs/lv2soname
lv2soname_INSTALLDIR = $(bindir)

# extra files
DOCS = COPYING AUTHORS README ChangeLog
PCFILES = lv2-plugin.pc lv2-gui.pc
EXTRA_DIST = Doxyfile


# Do the magic
include Makefile.template

