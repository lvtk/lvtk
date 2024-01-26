
lvtk_libdir = get_option('libdir')

### LVTK Widgets
lvtk_widgets_sources = files('''
    ui/button.cpp
    ui/embed.cpp
    ui/entry.cpp
    ui/font.cpp
    ui/graphics.cpp
    ui/image.cpp
    ui/main.cpp
    ui/fitment.cpp
    ui/slider.cpp
    ui/style.cpp
    ui/view.cpp
    ui/widget.cpp
'''.split())

if host_machine.system() == 'darwin'
    lvtk_widgets_sources += 'ui/embed_mac.mm'
elif host_machine.system() == 'linux'
    lvtk_widgets_sources += 'ui/embed_x11.cpp'
elif host_machine.system() == 'windows'
    lvtk_widgets_sources += 'ui/embed_win32.cpp'
endif

lvtk_widgets_deps = [ lvtk_dep, pugl_dep, boost_dep, ui_apple_deps ]
add_project_arguments ('-DPUGL_STATIC', language : ['c', 'cpp'])

liblvtk_widgets = library ('lvtk-widgets-@0@'.format (lvtk_abi_version),
    lvtk_widgets_sources,
    c_args : [ '-DLVTK_BUILD' ],
    cpp_args : [ '-DLVTK_BUILD' ],
    dependencies : lvtk_widgets_deps,
    gnu_symbol_visibility : 'hidden',
    install : true,
    install_dir : lvtk_libdir)
lvtk_widgets_dep = declare_dependency (
    link_with : liblvtk_widgets,
    include_directories : [],
    dependencies : lvtk_widgets_deps)

subdir ('ui/res')

### LVTK Cairo
if pugl_cairo_dep.found()
    lvtk_cairo_deps = [ lvtk_widgets_dep, cairo_dep, pugl_cairo_dep ]
    liblvtk_cairo = library ('lvtk-cairo-@0@'.format (lvtk_abi_version),
        [ 'ui/cairo.cpp' ],
        gnu_symbol_visibility : 'hidden',
        link_with : [],
        include_directories : [],
        c_args : [ '-DLVTK_BUILD' ],
        cpp_args : [ '-DLVTK_BUILD' ],
        dependencies : lvtk_cairo_deps,
        install : true,
        install_dir : lvtk_libdir)
    lvtk_cairo_dep = declare_dependency (
        link_with : [ liblvtk_cairo ],
        include_directories : [],
        dependencies : lvtk_cairo_deps)
else
    lvtk_cairo_dep = dependency('', required : false)
endif

### LVTK Hosting
# The lvtk_host_dep is declared for convenience to pull in lilv and suil
if lilv_dep.found() and suil_dep.found()
    lvtk_host_dep = declare_dependency (
        link_with : [],
        include_directories : [],
        dependencies : [ lvtk_dep, lilv_dep, suil_dep ])
else
    lvtk_host_dep = dependency('', required : false)
endif
