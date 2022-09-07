
pkg = import ('pkgconfig')
pkg.generate (
    name        : 'LVTK',
    filebase    : 'lvtk-3',
    version     : meson.project_version(),
    description : 'LV2 Plugin C++ Library',
    subdirs     : 'lvtk-3',
    requires    : [])
pkg.generate (liblvtk_ui,
    name        : 'LVTK UI',
    filebase    : 'lvtk-ui-3',
    version     : meson.project_version(),
    description : 'LV2 C++ Gui librrary',
    subdirs     : 'lvtk-3',
    requires    : [])
pkg.generate (liblvtk_nanovg,
    name        : 'LVTK NanoVG',
    filebase    : 'lvtk-nanovg-3',
    version     : meson.project_version(),
    description : 'NanoVG backend for LVTK',
    subdirs     : 'lvtk-3',
    requires    : [])
