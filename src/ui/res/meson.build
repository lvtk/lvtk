
lvtk_res_sources = []

lvtk_res_gen = generator (find_program('python3', 'python'),
    output : [ '@PLAINNAME@.h' ],
    arguments : [ meson.project_source_root() / 'scripts/bin2c.py', 
                  '@INPUT@', '@OUTPUT@', '@INPUT@' ])

lvtk_res_sources += lvtk_res_gen.process ('Roboto-Regular.ttf')
lvtk_res_includes = include_directories('.')
