lvtk_installer_plugins_lv2dir = '/Library/Audio/Plug-Ins/LV2'
lvtk_plugins_operations = '''<Operations>
    <Operation name="Mkdir">
        <Argument>@0@</Argument>
    </Operation>'''.format(lvtk_installer_plugins_lv2dir)

lvtk_plugins_operations += '''
    <Operation name="Extract">
        <Argument>@0@</Argument>
    </Operation>
'''.format('/Library/Audio/Plug-Ins')

lvtk_plugins_operations += '</Operations>'

foreach m : [ 'package.xml' ]
    configure_file (input: m, output: m,
        install: true, install_dir: lvtk_deploy_pluginsdir / 'meta',
        configuration: {
            'VERSION': lvtk_version_with_build,
            'RELEASEDATE': lvtk_release_date,
            'OPERATIONS': lvtk_plugins_operations
        })
endforeach
