
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

lvtk_widgets_deps = [ lvtk_dep, pugl_dep, boost_dep ]
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

### LVTK OpenGL
subdir ('ui/res')
lvtk_gl_sources = ['ui/nanovg.cpp', 'ui/opengl.cpp', lvtk_res_sources ]
lvtk_gl_c_args = ['-DLVTK_BUILD']
lvtk_gl_cpp_args = lvtk_gl_c_args
subdir ('nanovg')

lvtk_opengl_deps = [ lvtk_widgets_dep, pugl_gl_dep, dl_dep ]

liblvtk_opengl = library ('lvtk-opengl-@0@'.format (lvtk_abi_version),
    lvtk_gl_sources,
    gnu_symbol_visibility : 'hidden',
    link_with : [],
    include_directories : [ 'nanovg' ],
    c_args : [ lvtk_gl_c_args ],
    cpp_args : [ lvtk_gl_cpp_args ],
    dependencies : [ lvtk_opengl_deps ],
    install : true,
    install_dir : lvtk_libdir)
lvtk_opengl_dep = declare_dependency (
    link_with : [ liblvtk_opengl ],
    include_directories : [],
    dependencies : lvtk_opengl_deps)

### LVTK Hosting
if lilv_dep.found() and suil_dep.found()
    lvtk_host_sources = files ('''
        host/world.cpp
    '''.split())
    liblvtk_host = library ('lvtk-host-@0@'.format (lvtk_abi_version), 
        lvtk_host_sources,
        gnu_symbol_visibility : 'hidden',
        dependencies : [ lvtk_dep, lilv_dep, suil_dep ],
        install : true,
        install_dir : lvtk_libdir)
    lvtk_host_dep = declare_dependency (
        link_with : [ liblvtk_host ],
        include_directories : [],
        dependencies : [ lvtk_dep, lilv_dep, suil_dep ])
endif
