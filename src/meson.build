
lvtk_ui_sources = '''
    ui/embed.cpp
    ui/font.cpp
    ui/graphics.cpp
    ui/image.cpp
    ui/main.cpp
    ui/style.cpp
    ui/view.cpp
    ui/widget.cpp
'''.split()

lvtk_ui_deps = [ lvtk_dep, pugl_dep ]
add_project_arguments ('-DPUGL_STATIC', language : ['c', 'cpp'])

liblvtk_ui = library ('lvtk-ui-3',
    lvtk_ui_sources,
    c_args : [ '-DLVTK_BUILD' ],
    cpp_args : [ '-DLVTK_BUILD' ],
    version : meson.project_version(),
    dependencies : lvtk_ui_deps,
    gnu_symbol_visibility : 'hidden',
    install : true)
lvtk_ui_dep = declare_dependency (
    link_with : liblvtk_ui,
    include_directories : [],
    dependencies : lvtk_ui_deps)

subdir ('ui/res')
lvtk_gl_sources = ['ui/nanovg.cpp', 'ui/opengl.cpp', lvtk_res_sources ]
lvtk_gl_c_args = ['-DLVTK_BUILD']
lvtk_gl_cpp_args = lvtk_gl_c_args
subdir ('nanovg')

lvtk_gl_deps = [ lvtk_ui_dep, pugl_gl_dep, dl_dep ]

liblvtk_gl = library ('lvtk-gl-3',
    lvtk_gl_sources,
    version : meson.project_version(),
    gnu_symbol_visibility : 'hidden',
    link_with : [],
    include_directories : [ 'nanovg' ],
    c_args : [ lvtk_gl_c_args ],
    cpp_args : [ lvtk_gl_cpp_args ],
    dependencies : [ lvtk_gl_deps ],
    install : true)
lvtk_gl_dep = declare_dependency (
    link_with : [ liblvtk_gl ],
    include_directories : [],
    dependencies : lvtk_gl_deps)

if lilv_dep.found() and suil_dep.found()
    lvtk_host_sources = files ('''
        host/world.cpp
    '''.split())
    liblvtk_host = library ('lvtk-host', 
        lvtk_host_sources,
        version : meson.project_version(),
        gnu_symbol_visibility : 'hidden',
        dependencies : [ lvtk_dep, lilv_dep, suil_dep ],
        install : true)
    lvtk_host_dep = declare_dependency (
        link_with : [ liblvtk_host ],
        include_directories : [],
        dependencies : [ lvtk_dep, lilv_dep, suil_dep ])
endif
