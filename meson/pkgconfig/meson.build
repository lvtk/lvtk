
pkg = import ('pkgconfig')
pkg.generate (
    name        : 'LVTK',
    filebase    : 'lvtk-@0@'.format (lvtk_abi_version),
    version     : meson.project_version(),
    description : 'LV2 Plugin C++ Library',
    subdirs     : 'lvtk-@0@'.format (lvtk_abi_version),
    install_dir : get_option ('libdir') / 'pkgconfig',
    requires    : [])
pkg.generate (liblvtk_widgets,
    name        : 'LVTK Widgets',
    filebase    : 'lvtk-widgets-@0@'.format (lvtk_abi_version),
    version     : meson.project_version(),
    description : 'LVTK Widget Library',
    subdirs     : 'lvtk-@0@'.format (lvtk_abi_version),
    install_dir : get_option ('libdir') / 'pkgconfig',
    requires    : [])
pkg.generate (liblvtk_opengl,
    name        : 'LVTK OpenGL',
    filebase    : 'lvtk-opengl-@0@'.format (lvtk_abi_version),
    version     : meson.project_version(),
    description : 'LVTK OpenGL backend for Widgets library',
    subdirs     : 'lvtk-@0@'.format (lvtk_abi_version),
    install_dir : get_option ('libdir') / 'pkgconfig',
    requires    : [])
pkg.generate (
    name        : 'LVTK Hosting Library',
    filebase    : 'lvtk-host-@0@'.format (lvtk_abi_version),
    version     : meson.project_version(),
    description : 'LV2 hosting library',
    subdirs     : 'lvtk-@0@'.format (lvtk_abi_version),
    install_dir : get_option ('libdir') / 'pkgconfig',
    requires    : [ 'lv2', 'lilv-0', 'suil-0' ])
