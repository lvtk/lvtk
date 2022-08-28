
fs = import ('fs')

plugin_install_dir = get_option('libdir') / 'lv2' / 'lvtk.lv2'

plugin = shared_module ('volume', 'volume.cpp',
    name_prefix : '',
    dependencies : [ lvtk_dep ],
    install : true,
    install_dir : plugin_install_dir,
    gnu_symbol_visibility : 'hidden'
)

configure_file (
    input : 'manifest.ttl.in',
    output : 'manifest.ttl',
    configuration : {
        'BINARY' : fs.name (plugin.full_path())
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
