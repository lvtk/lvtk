
pkg = import ('pkgconfig')
pkg.generate (
    name        : 'LVTK',
    filebase    : 'lvtk-3',
    version     : meson.project_version(),
    description : 'LV2 Plugin C++ Library',
    subdirs     : 'lvtk-3',
    requires    : [])
pkg.generate (liblvtk_gl,
    name        : 'LVTK OpenGL',
    filebase    : 'lvtk-gl-3',
    version     : meson.project_version(),
    description : 'OpenGL backend for LVTK',
    subdirs     : 'lvtk-3',
    requires    : [])
