doxygen = find_program ('doxygen', required : get_option('docs'))
dot = find_program ('dot', required : false)
sphinx_build = find_program ('sphinx-build', required : get_option('docs'))

configure_file (
    input : 'reference.doxygen.in',
    output : 'reference.doxygen',
    configuration : {
        'LVTK_SRCDIR'           : meson.project_source_root(),
        'LVTK_DOC_DIR'          : meson.project_build_root() / 'doc/api',
        'LVTK_VERSION'          : meson.project_version()
    })
