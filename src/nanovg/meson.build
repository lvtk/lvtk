lvtk_nanovg_sources = '''
    nanovg/nanovg.c
    nanovg/nanovg_gl.c
'''.split()

if host_machine.system() == 'windows'
    lvtk_nanovg_sources += 'nanovg/glad.c'
endif

lvtk_nanovg_gl_type = host_machine.system() == 'darwin' ? 'NANOVG_GL2' : 'NANOVG_GL3'
lvtk_nanovg_c_args = [ '-DNVG_NO_STB=1', '-D@0@=1'.format (lvtk_nanovg_gl_type) ]

lvtk_nanovg_cpp_args = lvtk_nanovg_c_args
if host_machine.system() == 'linux'
    lvtk_nanovg_c_args += '-Wno-implicit-function-declaration'
endif

lvtk_gl_sources += lvtk_nanovg_sources
lvtk_gl_c_args += lvtk_nanovg_c_args
lvtk_gl_cpp_args += lvtk_nanovg_cpp_args
