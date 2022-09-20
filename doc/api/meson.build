# Copyright 2021 David Robillard <d@drobilla.net>
# SPDX-License-Identifier: CC0-1.0 OR ISC

cpp_doxyfile = configure_file (configuration: {
      'LVTK_SRCDIR' : meson.project_source_root(),
      'DOX_OUTPUT' : meson.current_build_dir() / '../html'
    },
    input: '../Doxyfile.in',
    output: 'Doxyfile')

cpp_index_xml = custom_target (
    'cpp-index.xml',
    command: [doxygen, '@INPUT0@'],
    input: [cpp_doxyfile, lvtk_headers],
    output: 'index.xml',
    build_by_default : true)
