configure_file (
    input : 'reference.doxygen.in',
    output : 'reference.doxygen',
    configuration : {
        'LVTK_SRCDIR'           : meson.current_source_dir(),
        'LVTK_DOC_DIR'          : meson.current_build_dir() / 'doc/api',
        'LVTK_VERSION'          : meson.project_version()
    })
