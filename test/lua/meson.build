if lua_dep.found() and lua_program.found()

lvtk_lua_tests = '''
    loadall
    bytes
    include
    midi
    object
    promise
    round

    PluginInfo
    World
'''.split()
foreach key : lvtk_lua_tests
    luafile = '@0@'.format(meson.project_source_root()) / 'test' / 'lua' / '@0@.lua'.format(key)
    test (key, lua_program, args : [ luafile ], suite: 'lua',
        env : ['LUA_PATH=@0@/bindings/?.lua'.format (meson.project_source_root()), 
               'LUA_CPATH=@0@/bindings/?.so'.format (meson.project_build_root()),
               'LV2_PATH=@0@'.format (meson.project_build_root())])
endforeach

endif
