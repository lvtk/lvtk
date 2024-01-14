cc = meson.get_compiler ('c')
cpp = meson.get_compiler ('cpp')
dl_dep = cc.find_library ('dl', required: false)
reuse_program = find_program ('reuse', required : false)

threads_dep = dependency ('threads')

cocoa_dep = dependency('', required : false)
corevideo_dep = dependency('', required : false)
apple_deps = []
ui_apple_deps = []

if host_machine.system() == 'darwin'
    cocoa_dep = dependency('appleframeworks', required: true, 
        modules: ['Foundation'])
    corevideo_dep = dependency('appleframeworks', required: false, 
        modules: ['CoreVideo', 'CoreGraphics', 'Cocoa'])
    apple_deps = [cocoa_dep]
    ui_apple_deps = [apple_deps, corevideo_dep]
endif

### Lv2 Core
lv2_dep = dependency ('lv2',
    fallback : [ 'lv2', 'lv2_dep' ],
    version : '>= 1.16.0', 
    required : true,
    default_options : [
        'tests=disabled',
        'plugins=disabled',
        'docs=disabled'
    ])

lvtk_lv2dir = get_option('lv2dir')
if lvtk_lv2dir == ''
  if target_machine.system() == 'darwin'
    lvtk_lv2dir = '/Library/Audio/Plug-Ins/LV2'
  elif target_machine.system() == 'haiku'
    lvtk_lv2dir = '/boot/common/add-ons/lv2'
  elif target_machine.system() == 'windows'
    lvtk_lv2dir = 'C:/Program Files/Common Files/LV2'
  else
    lvtk_lv2dir = get_option('prefix') / get_option('libdir') / 'lv2'
  endif
endif

### Pugl
pugl_opts = [
    'tests=disabled',
    'examples=disabled',
    'cairo=disabled',
    'vulkan=disabled',
    'stub=false',
    'docs=disabled'
]

pugl_dep = dependency ('pugl-0', 
    fallback : [ 'pugl', 'pugl_dep' ],
    version : '>= 0.4.0',
    required : true,
    default_options : pugl_opts
)

pugl_gl_dep = dependency ('pugl-gl-0', 
    fallback : [ 'pugl' ],
    version : '>= 0.4.0',
    default_options : pugl_opts,
    required : true
)

### OpenGL

opengl_dep = dependency ('GL', include_type : 'system')

### Lilv & Suil
lilv_dep = dependency ('lilv-0', required : false)
suil_dep = dependency ('suil-0', required : false)

### Boost
lvtk_build_widgets = true
boost_dep = dependency ('boost', required : lvtk_build_widgets or get_option('test').enabled())

### Lua if wanted
lua_dep = dependency ('', required : false)
lua_program = find_program ('', required : false)
lua_version = '0.0.0'

if not get_option ('bindings').disabled()
  ## Lua: Use either lua 5.3 or 5.4 depending on what's on the
  # system.  If lua not found. Use 5.4.

  lvtk_lua_system = false

  if host_machine.system() != 'windows'
    lua_dep = dependency('', required : false)
    lua_version = ''
    lua_program = find_program ('lua5.4', required : false)

    if not lua_program.found()
        lua_program = find_program('lua5.3', required : false)
    endif

    if not lua_program.found()
        lua_program = find_program('lua', required : false)
    endif

    if lua_program.found()
        pymod = import('python')
        lua_version = run_command(pymod.find_installation('python3'), '../../scripts/luavers.py',
            lua_program, check : true).stdout().strip()
        if not lua_version.version_compare ('>= 5.3')
          lvtk_lua_system = false
        else
          lvtk_lua_system = true
        endif
    endif
  endif

  if not lvtk_lua_system
    lua_proj = subproject ('lua', 
      default_options : [
        'loadlib=disabled',
        'interpreter=true',
        'line_editing=disabled'
      ])

    lua_dep = lua_proj.get_variable ('lua_dep')
    lua_version = '5.4'
    lua_program = lua_proj.get_variable ('lua_exe')
    lvtk_lua_system = false
  endif
endif
