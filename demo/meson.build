lvtk_demo_sources = '''
    video/image_source.cpp
    video/smpte_bars.cpp
    video/solid_source.cpp
    video/engine.cpp
'''.split()

lvtk_demo_includes = include_directories ('.')

liblvtk_demo = static_library ('lvtk-demo',
    lvtk_demo_sources,
    include_directories : [ lvtk_demo_includes ],
    dependencies : [ evg_dep, lvtk_nanovg_dep, xcb_dep ],
    install : false
)
