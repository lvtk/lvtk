lvtk_nanovg_sources = '''
    nanovg.c
    nanovg_gl.c
'''.split()

libnanovg = static_library ('nanovg',
    lvtk_nanovg_sources,
    c_args : [ '-DNANOVG_GL3=1', '-Wno-implicit-function-declaration' ],
    gnu_symbol_visibility : 'hidden',
    include_directories : [ '.' ],
    dependencies : [ pugl_gl_dep ],
    install : false)
