project ('lvtk', ['cpp', 'c'], 
    version : '3.0.0',
    meson_version : '>= 0.64.0',
    license : 'ISC',
    default_options: [
        'b_ndebug=if-release',
        'buildtype=release',
        'c_std=c99',
        'cpp_std=c++17',
    ])

fs = import ('fs')

lvtk_abi_version = '3.0'

# $ find include -name "*.hpp"
lvtk_headers = files ('''
    include/lvtk/lvtk.hpp
    include/lvtk/weak_ref.hpp
    include/lvtk/ui.hpp
    include/lvtk/context.hpp
    include/lvtk/ext/extension.hpp
    include/lvtk/ext/touch.hpp
    include/lvtk/ext/state.hpp
    include/lvtk/ext/resize_port.hpp
    include/lvtk/ext/idle.hpp
    include/lvtk/ext/port_subscribe.hpp
    include/lvtk/ext/port_map.hpp
    include/lvtk/ext/instance_access.hpp
    include/lvtk/ext/options.hpp
    include/lvtk/ext/log.hpp
    include/lvtk/ext/parent.hpp
    include/lvtk/ext/resize.hpp
    include/lvtk/ext/urid.hpp
    include/lvtk/ext/bufsize.hpp
    include/lvtk/ext/data_access.hpp
    include/lvtk/ext/atom.hpp
    include/lvtk/ext/worker.hpp
    include/lvtk/ext/show.hpp
    include/lvtk/options.hpp
    include/lvtk/plugin.hpp
    include/lvtk/symbols.hpp
    include/lvtk/optional.hpp
    include/lvtk/memory.hpp
    include/lvtk/spin_lock.hpp
    include/lvtk/string.hpp
    include/lvtk/dynmanifest.hpp
'''.split())

if host_machine.system() == 'darwin'
    add_languages (['objcpp', 'objc' ])
endif

subdir ('meson/deps')
subdir ('meson/supress')

### Header only deps
lvtk_dep = declare_dependency (
    include_directories : [ 'include' ],
    dependencies : [ lv2_dep ])
lvtk_internal_dep = declare_dependency (
    include_directories : [ 'src' ],
    dependencies : lvtk_dep)

subdir ('src')

### Install Headers and PC
install_subdir ('include' / 'lvtk', install_dir : get_option('includedir') / 'lvtk-@0@'.format(lvtk_abi_version))
subdir ('meson/pkgconfig')

### Tests
if not get_option('test').disabled()
    subdir('test')
endif

### Docs
if not get_option('doc').disabled()
    subdir ('doc')
endif

### Summary
summary ('Docs', get_option('doc'), bool_yn: true)
summary ('Tests', get_option('test'), bool_yn: true)
summary ('Prefix', get_option('prefix'), section: 'Paths')
summary ('Headers', get_option('prefix') / get_option('includedir'), section: 'Paths')
summary ('Libraries', get_option('prefix') / get_option('libdir'), section: 'Paths')
summary ('Programs', get_option('prefix') / get_option('bindir'), section: 'Paths')
summary ('LV2', get_option('prefix') / lvtk_lv2dir, section: 'Paths')
