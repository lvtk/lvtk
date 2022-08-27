project ('lvtk', 'cpp', version : '2.0.0',
    meson_version : '>= 0.49.0',
    default_options : [ 'cpp_std=c++14' ])

lv2_dep = dependency ('lv2', version : '>= 1.15.4', required : true)
doxygen = find_program ('doxygen')
dot = find_program ('dot')
cppunit_dep = dependency ('cppunit', required : false)

install_subdir ('lvtk', install_dir : 'include' / 'lvtk-2')

pkg = import ('pkgconfig')
pkg.generate (
    name : 'LVTK',
    filebase : 'lvtk-2',
    version : meson.project_version(),
    description : 'LV2 Toolkit Plugin Library',
    subdirs : 'lvtk-2',
    requires : [ lv2_dep ]
)

lvtk_dep = declare_dependency (
    include_directories : '.',
    dependencies : [ lv2_dep ])

if doxygen.found() and dot.found()
    subdir ('doc')
endif

if cppunit_dep.found()
    subdir ('tests')
endif
