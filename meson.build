project ('lvtk', ['cpp', 'c'], version : '3.0.0',
    meson_version : '>= 0.56.0',
    default_options : [ 'cpp_std=c++17' ])

subdir ('meson/deps')

lvtk_dep = declare_dependency (
    include_directories : [ 'include' ],
    dependencies : [ lv2_dep ])
lvtk_internal_dep = declare_dependency (
    include_directories : [ 'src' ],
    dependencies : lvtk_dep)

subdir ('src')
subdir ('util/lualv')
subdir ('bindings')
lualv_prog = executable ('lualv',
    'util/lualv/main.cpp',
    win_subsystem : 'console',
    link_with : liblualv_exe,
    dependencies : lualv_console_deps,
    c_args : lualv_console_c_args,
    cpp_args : lualv_console_cpp_args
)

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
