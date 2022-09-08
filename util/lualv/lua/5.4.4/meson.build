
# lualv_versions = meson.project_version().split('.')
lualv_versions = '5.4.4'.split('.')
# Override the c_std and warning_level of any superproject.
project_options = ['c_std=c99', 'warning_level=2']
cc = meson.get_compiler ('c')

# Skip bogus warning.
if cc.get_id() == 'clang'
    lualv_c_args += '-Wno-string-plus-int'
elif cc.get_id() == 'gcc'
    lualv_c_args += '-Wno-stringop-overflow'
endif

# Platform-specific defines.
is_posix = host_machine.system() not in ['windows', 'emscripten', 'android']
if is_posix
    lualv_c_args += '-DLUA_USE_POSIX'
elif get_option('default_library') != 'static'
    lualv_c_args += '-DLUA_BUILD_AS_DLL'
endif

# Library dependencies.
lualv_lib_deps = [cc.find_library('m', required: false)]

# if get_option('loadlib')
if true
    dl_dep = cc.find_library('dl', required: false)
    lualv_lib_deps += [dl_dep]
    lualv_c_args += '-DLUA_USE_DLOPEN'
endif

# Interpreter dependencies.
lualv_exe_deps = []
lualv_exe_args = []

if get_option('readline')
    readline_dep = dependency('readline', required: false)
    if not readline_dep.found()
        readline_dep = dependency('libedit', required: false)
        if not readline_dep.found()
            message('''Didn't find readline or libedit, lualv line editing disabled.''')
            readline_dep = dependency('', required : false)
        endif
    endif
    lualv_exe_deps += [ readline_dep ]
    lualv_exe_args += [ '-DLUA_USE_READLINE' ]
endif

# Targets.
lualv_lib = library ('lualv',
    'src/lapi.c',
    'src/lauxlib.c',
    'src/lbaselib.c',
    'src/lcode.c',
    'src/lcorolib.c',
    'src/lctype.c',
    'src/ldblib.c',
    'src/ldebug.c',
    'src/ldo.c',
    'src/ldump.c',
    'src/lfunc.c',
    'src/lgc.c',
    'src/linit.c',
    'src/liolib.c',
    'src/llex.c',
    'src/lmathlib.c',
    'src/lmem.c',
    'src/loadlib.c',
    'src/lobject.c',
    'src/lopcodes.c',
    'src/loslib.c',
    'src/lparser.c',
    'src/lstate.c',
    'src/lstring.c',
    'src/lstrlib.c',
    'src/ltable.c',
    'src/ltablib.c',
    'src/ltm.c',
    'src/lundump.c',
    'src/lutf8lib.c',
    'src/lvm.c',
    'src/lzio.c',
    dependencies: lualv_lib_deps,
    override_options: project_options,
    implicit_include_directories: false,
    c_args : lualv_c_args,
    install : true
)

lua_link = lualv_lib

lualv_includes = include_directories ('src')
lualv_dep = declare_dependency (link_with: lualv_lib, 
    include_directories: lualv_includes)
