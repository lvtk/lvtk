
lvtk_ui_sources = '''
    ui/main.cpp
    ui/view.cpp
    ui/widget.cpp
    ui.cpp
'''.split()

lvtk_ui_deps = [ lvtk_dep, puglpp_dep ]
liblvtk_ui = library ('lvtk-ui-2',
    lvtk_ui_sources,
    dependencies : lvtk_ui_deps)
lvtk_ui_dep = declare_dependency (
    link_with : liblvtk_ui,
    include_directories : [ '.' ],
    dependencies : lvtk_ui_deps)

subdir ('nanovg')
lvtk_gl_deps = [ lvtk_ui_dep, lvtk_nanovg_dep, pugl_gl_dep ]
liblvtk_gl = library ('lvtk-gl-2',
    'ui/opengl.cpp',
    link_with : [],
    dependencies : lvtk_gl_deps)
lvtk_gl_dep = declare_dependency (
    link_with : [ liblvtk_gl ],
    include_directories : [ '.' ],
    dependencies : lvtk_gl_deps)
