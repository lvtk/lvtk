lvtk_nanovg_sources = '''
    nanovg.c
    nanovg_gl.c
'''.split()

liblvtk_nanovg = static_library ('lvtk-nanovg-0',
    lvtk_nanovg_sources,
    c_args : [ '-DNANOVG_GL3=1', '-Wno-implicit-function-declaration' ],
    gnu_symbol_visibility : 'hidden',
    include_directories : [ '.' ],
    dependencies : [ lvtk_ui_dep, pugl_gl_dep ],
    install : false)
lvtk_nanovg_dep = declare_dependency (
    link_with : [ liblvtk_nanovg ],
    include_directories : [ '.' ],
    dependencies :  [ lvtk_ui_dep, pugl_gl_dep ])
