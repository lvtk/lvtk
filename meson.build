project ('lvtk', ['cpp', 'c'], version : '3.0.0',
    meson_version : '>= 0.53.0',
    default_options : [ 'cpp_std=c++17' ])

subdir ('meson/packages')

lvtk_dep = declare_dependency (
    include_directories : [ 'include', 'src' ],
    dependencies : [ lv2_dep ])

subdir ('src')
subdir ('lvtk.lv2')
subdir ('demo')

install_subdir ('include' / 'lvtk', install_dir : 'include' / 'lvtk-3')

pkg = import ('pkgconfig')
pkg.generate (
    name        : 'LVTK',
    filebase    : 'lvtk-3',
    version     : meson.project_version(),
    description : 'LV2 C++ templates',
    subdirs     : 'lvtk-3',
    requires    : [])
pkg.generate (liblvtk_ui,
    name        : 'LVTK',
    filebase    : 'lvtk-ui-3',
    version     : meson.project_version(),
    description : 'LV2 UI Library',
    subdirs     : 'lvtk-3',
    requires    : [])
pkg.generate (liblvtk_gl,
    name        : 'LVTK',
    filebase    : 'lvtk-gl-3',
    version     : meson.project_version(),
    description : 'LVTK OpenGL Backend Library',
    subdirs     : 'lvtk-3',
    requires    : [])

if doxygen.found() and dot.found()
    subdir ('doc')
endif

if cppunit_dep.found()
    subdir ('tests')
endif
