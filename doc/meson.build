configure_file (
    input : 'reference.doxygen.in',
    output : 'reference.doxygen',
    configuration : {
        'LVTK_SRCDIR'           : meson.global_source_root(),
        'LVTK_DOC_DIR'          : meson.global_build_root() / 'doc/api',
        'LVTK_VERSION'          : meson.project_version()
    },
    install : false)
