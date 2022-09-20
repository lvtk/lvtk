project ('lvtk', ['cpp', 'c'], version : '3.0.0',
    meson_version : '>= 0.56.0',
    license: 'ISC',
    default_options: [
        'b_ndebug=if-release',
        'buildtype=release',
        'c_std=c99',
        'cpp_std=c++17',
    ])

subdir ('meson/deps')


if get_option('default_library') == 'both'
  if host_machine.system() == 'windows'
    error('default_library=both is not supported on Windows')
  endif
elif get_option('default_library') == 'static'
    add_project_arguments(['-DLVTK_STATIC'],language: ['c', 'cpp'])
endif

### Header only deps
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
# subdir ('lvtk.lv2')

### Demo
subdir ('demo')
executable (
    'lvtk-demo', 
    'demo/main.cpp',
    cpp_args : [ '-DLVTK_NO_SYMBOL_EXPORT' ],
    include_directories : [],
    link_with : [ liblvtk_demo ],
    dependencies : [ lvtk_gl_dep, threads_dep ],
    win_subsystem : 'windows',
    install : true)

### Headers and PC
install_subdir ('include' / 'lvtk', install_dir : 'include' / 'lvtk-3')
subdir ('meson/pkgconfig')

### Docs
if not get_option('docs').disabled()
    subdir ('doc')
endif

### Tests
if not get_option('unit-tests').disabled()
    subdir('test')
endif

### Summary
summary('Tests', not get_option('unit-tests').disabled(), bool_yn: true)

summary('Graphics', 'OpenGL', section : 'Gui')
summary('Demo', true, bool_yn: true, section : 'Gui')

summary('Prefix', get_option('prefix'), section: 'Paths')
summary('Headers', get_option('prefix') / get_option('includedir'), section: 'Paths')
summary('Libraries', get_option('prefix') / get_option('libdir'), section: 'Paths')
