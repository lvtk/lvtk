cc = meson.get_compiler ('c')
cpp = meson.get_compiler ('cpp')
dl_dep = cc.find_library ('dl', required: false)

threads_dep = dependency ('threads')

### Lv2 Core
lv2_dep = dependency ('lv2',
    fallback : [ 'lv2', 'lv2_dep' ],
    version : '>= 1.18.0', 
    required : true,
    default_options : [
        'tests=disabled',
        'plugins=disabled',
        'docs=disabled'
    ])

### Pugl
pugl_opts = [
    'tests=disabled',
    'examples=disabled',
    'cairo=disabled',
    'vulkan=disabled',
    'stub=false',
    'docs=disabled'
    # 'default_library=static'
]

pugl_dep = dependency ('pugl-0', 
    fallback : [ 'pugl', 'pugl_dep' ],
    version : '>= 0.4.0',
    required : true,
    default_options : pugl_opts
)

pugl_gl_dep = dependency ('pugl-gl-0', 
    fallback : [ 'pugl', 'gl_backend_dep' ],
    version : '>= 0.4.0',
    default_options : pugl_opts,
    required : true
)

### Lilv & Suil
lilv_dep = dependency ('lilv-0', 
    fallback : ['lilv', 'lilv_dep' ], 
    required : true,
    default_options : [
        'serd:tests=disabled'
    ])
suil_dep = dependency ('suil-0', fallback : ['suil', 'suil_dep'], required : true)

### Boost
boost_dep = (dependency ('boost', required : not get_option('unit-tests').disabled()))

## Lua: Try to use 5.3 lua headers on the system. 
#  Fallback to included 5.4 headers.
lua_version = ''
lua_program = find_program ('lua5.4', required : false)

if not lua_program.found()
    lua_program = find_program('lua5.3', required : false)
endif

if not lua_program.found()
    lua_program = find_program('lua')
endif

if lua_program.found()
    lua_version = run_command('python', '../../scripts/luavers.py',
        lua_program, check : true).stdout().strip()
    lua_dep = dependency ('lua@0@'.format(lua_version), required : false)
    if not lua_dep.found()
        lua_dep = dependency ('lua', pkgconfig : '--cflags', required : false)
    endif
endif

# true if using system
lvtk_lua_system = lua_dep.found() and lua_program.found()

#
