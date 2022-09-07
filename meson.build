project ('lvtk', ['cpp', 'c'], version : '3.0.0',
    meson_version : '>= 0.53.0',
    default_options : [ 'cpp_std=c++17' ])

subdir ('meson/deps')

lvtk_dep = declare_dependency (
    include_directories : [ 'include', 'src' ],
    dependencies : [ lv2_dep ])

subdir ('src')
subdir ('lvtk.lv2')
subdir ('demo')

install_subdir ('include' / 'lvtk', install_dir : 'include' / 'lvtk-3')

subdir ('meson/pkgconfig')

if doxygen.found() and dot.found()
    subdir ('doc')
endif

if cppunit_dep.found()
    subdir ('tests')
endif
