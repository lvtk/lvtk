
foreach n : 'import object'.split()
    configure_file (
        input       : '@0@.lua'.format(n),
        output      : '@0@.lua'.format(n),
        copy        : true,
        install     : true,
        install_dir : lualv_lua_modsubdir / 'lvtk'
    )
endforeach

foreach n : 'bytes midi round'.split()
    shared_module (n, [ '@0@.c'.format(n) ],
        name_prefix             : '',
        include_directories     : [],
        gnu_symbol_visibility   : 'hidden',
        dependencies            : [ lvtk_internal_dep, lualv_includes_dep ],
        install                 : true,
        install_dir             : lualv_lua_cmodsubdir / 'lvtk')
endforeach
