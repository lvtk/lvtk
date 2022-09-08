
lvtk_ui_sources = '''
    ui/main.cpp
    ui/view.cpp
    ui/widget.cpp
    ui/graphics.cpp
    ui.cpp
'''.split()

lvtk_ui_deps = [ lvtk_dep, puglpp_dep ]
liblvtk_ui = static_library ('lvtk-ui-3',
    lvtk_ui_sources,
    cpp_args : [ '-DLVTK_PROXY_IMPL' ],
    dependencies : lvtk_ui_deps,
    install : false)
lvtk_ui_dep = declare_dependency (
    link_with : liblvtk_ui,
    include_directories : [ '.' ],
    dependencies : lvtk_ui_deps)

subdir ('nanovg')
lvtk_nanovg_deps = [ lvtk_ui_dep, pugl_gl_dep ]
liblvtk_nanovg = library ('lvtk-nanovg-3',
    ['ui/nanovg.cpp'],
    link_with : [],
    cpp_args : [ lvtk_nanovg_c_args ],
    dependencies : [ lvtk_nanovg_deps ],
    objects : [ libnanovg.extract_all_objects (recursive : false),
                liblvtk_ui.extract_all_objects (recursive : false) ])
lvtk_nanovg_dep = declare_dependency (
    link_with : [ liblvtk_nanovg ],
    include_directories : [ '.' ],
    dependencies : lvtk_nanovg_deps)
