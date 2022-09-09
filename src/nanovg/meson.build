lvtk_nanovg_sources = '''
    nanovg.c
    nanovg_gl.c
'''.split()

lvtk_nanovg_c_args = [ '-DNANOVG_GL2=1' ]
lvtk_nanovg_cpp_args = lvtk_nanovg_c_args
if host_machine.system() == 'linux'
    lvtk_nanovg_c_args += '-Wno-implicit-function-declaration'
endif

libnanovg = static_library ('nanovg',
    lvtk_nanovg_sources,
    c_args :lvtk_nanovg_c_args,
    gnu_symbol_visibility : 'hidden',
    include_directories : [ '.', '..' ],
    dependencies : [ pugl_gl_dep ],
    install : false)
