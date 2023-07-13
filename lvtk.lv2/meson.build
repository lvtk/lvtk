
if get_option ('plugins').enabled()

fs = import ('fs')
lvtk_ui_type = 'X11UI'
if host_machine.system() == 'windows'
    lvtk_ui_type = 'WindowsUI'
elif host_machine.system() == 'darwin'
    lvtk_ui_type = 'CocoaUI'
endif

plugin_install_dir = lvtk_lv2dir / 'lvtk.lv2'

plugin = shared_module ('volume', 'volume.cpp',
    name_prefix : '',
    dependencies : [ lvtk_dep ],
    install : true,
    install_dir : plugin_install_dir,
    gnu_symbol_visibility : 'hidden'
)

ui = shared_module ('volume-ui', 'volume-ui.cpp',
    name_prefix : '',
    dependencies : [ lvtk_opengl_dep ],
    install : true,
    install_dir : plugin_install_dir,
    gnu_symbol_visibility : 'hidden'
)

configure_file (
    input : 'manifest.ttl.in',
    output : 'manifest.ttl',
    configuration : {
        'PLUGIN_BINARY' : fs.name (plugin.full_path()),
        'UI_BINARY' : fs.name (ui.full_path()),
        'UI_TYPE' : lvtk_ui_type
    },
    install : true,
    install_dir : plugin_install_dir
)

configure_file (
    input : 'volume.ttl',
    output : 'volume.ttl',
    copy : true,
    install : true,
    install_dir : plugin_install_dir
)

endif # plugins.enabled()
