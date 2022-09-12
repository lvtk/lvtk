
lvtk_demo_sources = '''
    demo.cpp
'''.split()

lvtk_demo_deps = [ lvtk_nanovg_dep ]

lvtk_demo_includes = include_directories ('.')

liblvtk_demo = static_library ('lvtk-demo',
    lvtk_demo_sources,
    include_directories : [ lvtk_demo_includes ],
    dependencies : [ lvtk_demo_deps ],
    install : false
)
