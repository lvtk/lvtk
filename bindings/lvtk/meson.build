

foreach n : 'import object'.split()
    configure_file (
        input       : '@0@.lua'.format(n),
        output      : '@0@.lua'.format(n),
        copy        : true,
        install     : true,
        install_dir : lvtk_lua_mod_subdir / 'lvtk'
    )
endforeach

foreach n : 'bytes midi round'.split()
    shared_module (n, [ '@0@.c'.format(n) ],
        name_prefix             : '',
        name_suffix             : lvtk_lua_modules_suffix,
        include_directories     : [ lvtk_lua_includes ],
        gnu_symbol_visibility   : 'hidden',
        dependencies            : [ lvtk_internal_dep, lvtk_lua_deps ],
        install                 : true,
        install_dir             : lvtk_lua_cmod_subdir / 'lvtk')
endforeach
