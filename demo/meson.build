
lvtk_demo_sources = []
lvtk_demo_deps = [ lvtk_nanovg_dep ]

lvtk_video_demo_sources = '''
    video/image_source.cpp
    video/smpte_bars.cpp
    video/solid_source.cpp
    video/engine.cpp
'''.split()

if host_machine.system() == 'linux' and evg_dep.found()
    lvtk_demo_sources += lvtk_video_demo_sources
    lvtk_demo_deps += evg_dep
endif

lvtk_demo_includes = include_directories ('.')

liblvtk_demo = static_library ('lvtk-demo',
    lvtk_demo_sources,
    include_directories : [ lvtk_demo_includes ],
    dependencies : [ lvtk_demo_deps ],
    install : false
)
