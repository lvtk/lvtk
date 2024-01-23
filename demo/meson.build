lvtk_demo_sources = '''
    demo.cpp
    entry.cpp
    images.cpp
    sliders.cpp
'''.split()

subdir('res')
lvtk_demo_sources += lvtk_demo_res_sources

lvtk_demo_deps = [ lvtk_host_dep ]

lvtk_demo_includes = include_directories ('.')

liblvtk_demo = static_library ('lvtk-demo',
    lvtk_demo_sources,
    include_directories : [ lvtk_demo_includes ],
    dependencies : [ lvtk_demo_deps ],
    install : false
)
