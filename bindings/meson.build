
# build low level modules
subdir ('lvtk')

# build high level class Extensions
lvtk_lua_cpp_sources = ['lvtk.cpp']
shared_module ('lvtk',
    lvtk_lua_cpp_sources,
    name_prefix: '',
    include_directories : [ ],
    gnu_symbol_visibility : 'hidden',
    dependencies: [ pugl_gl_dep, lvtk_internal_dep, lualv_includes_dep ],
    objects : [ liblvtk_nanovg.extract_all_objects (recursive : true),
                liblvtk_ui.extract_all_objects (recursive : true)],
    install : true,
    install_dir : lualv_lua_cmodsubdir)
