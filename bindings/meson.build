# TODO: msvc lua module building
if host_machine.system() != 'windows'

lvtk_lua_modules_suffix = []
if host_machine.system() == 'darwin'
    # lua needs it this way on OSX
    lvtk_lua_modules_suffix = 'so'
endif

lvtk_lua_includes = include_directories ('lua')
lvtk_lua_deps = []

if lvtk_lua_system
    lvtk_lua_deps += lua_dep
    lvtk_lua_version = lua_version
    lvtk_lua_version_full = '@0@.0'.format(lua_version)
else
    lvtk_lua_version = '5.4'
    lvtk_lua_version_full = '5.4.4'
    lvtk_lua_includes += include_directories (
        'lua/@0@'.format(lvtk_lua_version_full))
endif

# place where the LVTK bindings get installed.
lvtk_lua_cmod_subdir = '@0@/@1@/@2@'.format (
    get_option('libdir'),'lua', lvtk_lua_version)
lvtk_lua_mod_subdir = '@0@/@1@/@2@'.format (
    get_option('datadir'), 'lua', lvtk_lua_version)

lvtk_lua_cpp_args = []
if host_machine.system() == 'darwin'
    lvtk_lua_cpp_args += ['-Wno-unknown-warning-option'] # sol2
endif

# build low level modules
subdir ('lvtk')

# build high level class Extensions
lvtk_lua_cpp_sources = ['lvtk.cpp']
shared_module ('lvtk',
    lvtk_lua_cpp_sources,
    name_prefix: '',
    name_suffix : lvtk_lua_modules_suffix,
    include_directories : lvtk_lua_includes,
    cpp_args : lvtk_lua_cpp_args,
    gnu_symbol_visibility : 'hidden',
    dependencies: [ pugl_gl_dep, lvtk_internal_dep, lvtk_lua_deps, lvtk_host_dep ],
    objects : [ liblvtk_opengl.extract_all_objects (recursive : true),
                liblvtk_widgets.extract_all_objects (recursive : true)],
    install : true,
    install_dir : lvtk_lua_cmod_subdir)

endif # not msvc
