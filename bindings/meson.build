
lvtk_luadir_c = get_option('luadir_c')
if lvtk_luadir_c == 'auto'
    lvtk_luadir_c = get_option('libdir') / 'lua'
endif

lvtk_luadir = get_option('luadir')
if lvtk_luadir == 'auto'
    lvtk_luadir = get_option('datadir') / 'lua'
endif

# TODO: msvc lua module building

lvtk_lua_modules_suffix = []
if host_machine.system() == 'darwin'
    # lua needs it this way on OSX
    lvtk_lua_modules_suffix = 'so'
elif host_machine.system() == 'windows'
    # windows nees this too? failed tests complain
    # about missing *.so files
    lvtk_lua_modules_suffix = 'so'
endif

lvtk_lua_includes = [ include_directories ('lua') ]
lvtk_lua_deps = [ lua_dep ]

if lvtk_lua_system
    lvtk_lua_deps += []
    lvtk_lua_version = lua_version
    lvtk_lua_includes += include_directories (
        'lua/@0@'.format(lvtk_lua_version))
else
    lvtk_lua_deps += []
    lvtk_lua_version = '5.4'
    lvtk_lua_includes += include_directories (
        'lua/@0@'.format(lvtk_lua_version))
endif

# place where the LVTK bindings get installed.
lvtk_lua_cmod_subdir = '@0@/@1@'.format (lvtk_luadir_c, lvtk_lua_version)
lvtk_lua_mod_subdir  = '@0@/@1@'.format (lvtk_luadir,   lvtk_lua_version)

lvtk_lua_c_args = []
lvtk_lua_cpp_args = []

if host_machine.system() == 'darwin'
    lvtk_lua_cpp_args += ['-Wno-unknown-warning-option'] # sol2
elif host_machine.system() == 'windows'
    lvtk_lua_cpp_args += ['-DLUA_BUILD_AS_DLL=1']
    lvtk_lua_c_args += ['-DLUA_BUILD_AS_DLL=1']
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
    cpp_args : [ lvtk_lua_cpp_args ],
    gnu_symbol_visibility : 'hidden',
    dependencies: [ pugl_gl_dep, lvtk_internal_dep, lvtk_lua_deps, lvtk_host_dep, lvtk_widgets_dep, boost_dep ],
    objects : [ liblvtk_opengl.extract_all_objects (recursive : true),
                liblvtk_widgets.extract_all_objects (recursive : true)],
    install : true,
    install_dir : lvtk_lua_cmod_subdir)
