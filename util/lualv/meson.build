
lualv_exe_name = 'lualv'  # honestly, this is not a great name for
# basically a new programming language built on top of lua.

lualv_lua_version = '5.4'
lualv_lua_version_full = '5.4.4'
lualv_interpretor_version = '0.0.0'

# place where the LVTK bindings get installed. Techinically they,
# should work with any 5.4 interpretor.
lualv_lua_cmodsubdir = '@0@/@1@/@2@'.format (
    get_option('libdir'),'lualv', lualv_lua_version)
lualv_lua_modsubdir = '@0@/@1@/@2@'.format (
    get_option('datadir'), 'lualv', lualv_lua_version)

lualv_path_base = get_option('prefix') / lualv_lua_modsubdir
lualv_cpath_base = get_option('prefix') / lualv_lua_cmodsubdir

lualv_path = ''
lualv_cpath = ''

lualv_path += lualv_path_base + '/?.lua'
lualv_path += ';' + lualv_path_base + '/?/init.lua'
# vanilla lua also checks cpath for .lua files.
lualv_path += ';' + lualv_cpath_base + '/?.lua'
lualv_path += ';' + lualv_cpath_base + '/?/init.lua'
lualv_path += ';./?.lua;./?/init.lua'

lualv_cpath += lualv_cpath_base + '/?.so'
lualv_cpath += ';' + lualv_cpath_base + '/loadall.so'
lualv_cpath += ';./?.so'

lualv_c_args = []
lualv_c_args += [
    '-DLUA_PATH_DEFAULT="@0@"'.format(lualv_path),
    '-DLUA_CPATH_DEFAULT="@0@"'.format(lualv_cpath),
]

# placeholder paths. maybe these are actually the same path?
# need to get down-n-dirty in RND to find out.
# lualv_impath ....  `include` path
# lualv_incpath ..... `import` path

# build liblualv.
subdir('lua/@0@'.format (lualv_lua_version_full))
lua_internal_includes = [ lualv_includes, include_directories ('include')]
lualv_includes_dep = declare_dependency (
    include_directories : [ lua_internal_includes ])

# console
lualv_cpp_args = lualv_c_args

lualv_console_deps = [ lualv_dep, lualv_includes_dep, lualv_exe_deps ]
lualv_console_c_args = [ lualv_exe_args, lualv_c_args ]
lualv_console_cpp_args = [ lualv_cpp_args, lualv_exe_args ]

liblualv_exe = static_library ('lualv-exe',
    '''lua.cpp'''.split(),
    include_directories : ['.'],
    dependencies : lualv_console_deps,
    c_args : lualv_console_c_args,
    cpp_args : lualv_console_cpp_args,
    link_with : [],
    install : false)
