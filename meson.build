project ('lvtk', ['cpp', 'c'], 
    version : '3.0.0',
    meson_version : '>= 0.56.0',
    license : 'ISC',
    default_options: [
        'b_ndebug=if-release',
        'buildtype=release',
        'c_std=c99',
        'cpp_std=c++17',
    ])

if host_machine.system() == 'darwin'
    add_languages (['objcpp', 'objc' ])
endif

date_py = files ('scripts/date.py')
lvtk_release_date = run_command ('python', date_py).stdout().strip()
version_py = files ('scripts/version.py')
lvtk_version_with_build = run_command ('python', 
    [version_py, '--build --ignore-dirty --cwd=.'.split()]).stdout().strip()
lvtk_build_number = run_command ('python', 
    [version_py, '--no-version --build --ignore-dirty --cwd=.'.split()]).stdout().strip()

lvtk_abi_version = '3.0'

lvtk_headers = files ('''
    include/lvtk/lvtk.hpp
    include/lvtk/string.hpp
    include/lvtk/ui/align.hpp
    include/lvtk/ui/widget.hpp
'''.split())

if not get_option ('onlydocs')

subdir ('meson/deps')
subdir ('meson/supress')

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
if not get_option('bindings').disabled()
subdir ('bindings')
endif

### Finals
subdir ('lvtk.lv2')

### Demo
if get_option('demo').enabled()
subdir ('demo')
executable (
    'lvtk-demo', 
    'demo/main.cpp',
    cpp_args : [ '-DLVTK_NO_SYMBOL_EXPORT' ],
    include_directories : [],
    link_with : [ liblvtk_demo ],
    dependencies : [ lvtk_opengl_dep, threads_dep ],
    win_subsystem : 'windows',
    install : true)
endif

### Install Headers and PC

install_subdir ('include' / 'lvtk', install_dir : get_option('includedir') / 'lvtk-@0@'.format(lvtk_abi_version))
subdir ('meson/pkgconfig')

### Tests
if not get_option('test').disabled()
    subdir('test')
endif

endif # only docs

### Docs
if not get_option('doc').disabled()
    subdir ('doc')
endif

### Summary
summary('Docs', not get_option('doc').disabled(), bool_yn: true)

if not get_option ('onlydocs')
summary('Tests', not get_option('test').disabled(), bool_yn: true)

summary('Modules', host_machine.system() != 'windows', section : 'Lua', bool_yn: true)
summary('Tests', lua_program.found() and not get_option('test').disabled(), 
    section : 'Lua', bool_yn: true)
summary('Version', lua_version, section : 'Lua')

summary('Graphics', 'OpenGL', section : 'Gui')
summary('Demo', true, bool_yn: true, section : 'Gui')

summary('Prefix', get_option('prefix'), section: 'Paths')
summary('Headers', get_option('prefix') / get_option('includedir'), section: 'Paths')
summary('Libraries', get_option('prefix') / get_option('libdir'), section: 'Paths')
summary('Programs', get_option('prefix') / get_option('bindir'), section: 'Paths')
summary('LV2', get_option('prefix') / lvtk_lv2dir, section: 'Paths')

endif
