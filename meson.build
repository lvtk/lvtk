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

### shared libs
subdir ('src')
subdir ('bindings')

### Finals
subdir ('lvtk.lv2')

subdir ('demo')
executable (
    'lvtk-demo', 
    'demo/main.cpp',
    cpp_args : [ '-DLVTK_NO_SYMBOL_EXPORT' ],
    include_directories : [],
    link_with : [ liblvtk_demo ],
    dependencies : [ lvtk_nanovg_dep, evg_dep, threads_dep ],
    install : true)

### 
install_subdir ('include' / 'lvtk', install_dir : 'include' / 'lvtk-3')
subdir ('meson/pkgconfig')

if doxygen.found() and dot.found()
    subdir ('doc')
endif

subdir ('test')
