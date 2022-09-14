
lvtk_ui_sources = '''
    ui/main.cpp
    ui/view.cpp
    ui/widget.cpp
    ui/graphics.cpp
    ui/embed.cpp
    ui.cpp
'''.split()

lvtk_ui_deps = [ lvtk_dep, puglpp_dep ]
liblvtk_ui = static_library ('lvtk-ui-3',
    lvtk_ui_sources,
    cpp_args : [],
    dependencies : lvtk_ui_deps,
    gnu_symbol_visibility : 'hidden',
    install : false)
lvtk_ui_dep = declare_dependency (
    link_with : liblvtk_ui,
    include_directories : [ '.' ],
    dependencies : lvtk_ui_deps)

subdir ('ui/res')
subdir ('nanovg')
lvtk_gl_deps = [ lvtk_ui_dep, pugl_gl_dep, dl_dep ]
liblvtk_gl = library ('lvtk-gl-3',
    ['ui/nanovg.cpp', 'ui/opengl.cpp', lvtk_res_sources ],
    version : meson.project_version(),
    link_with : [],
    gnu_symbol_visibility : 'default',
    cpp_args : [ lvtk_nanovg_cpp_args ],
    dependencies : [ lvtk_gl_deps ],
    objects : [ libnanovg.extract_all_objects (recursive : false),
                liblvtk_ui.extract_all_objects (recursive : false) ],
    install : true)
lvtk_gl_dep = declare_dependency (
    link_with : [ liblvtk_gl ],
    include_directories : [ '.' ],
    dependencies : lvtk_gl_deps)
