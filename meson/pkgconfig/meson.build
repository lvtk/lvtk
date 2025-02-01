
pkg = import ('pkgconfig')
pkg.generate (
    name        : 'LVTK',
    filebase    : 'lvtk-@0@'.format (lvtk_abi_version),
    version     : meson.project_version(),
    description : 'LV2 Plugin C++ Library',
    subdirs     : 'lvtk-@0@'.format (lvtk_abi_version),
    install_dir : get_option ('libdir') / 'pkgconfig',
    requires    : [])
