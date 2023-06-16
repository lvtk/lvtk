
lv2_bundles = '''
    atom.lv2
    worker.lv2
    patch.lv2
    state.lv2
    resize-port.lv2
    presets.lv2
    options.lv2
    schemas.lv2
    ui.lv2
    parameters.lv2
    units.lv2
    log.lv2
    morph.lv2
    midi.lv2
    core.lv2
    event.lv2
    buf-size.lv2
    dynmanifest.lv2
    port-groups.lv2
    time.lv2
    data-access.lv2
    urid.lv2
    port-props.lv2
    instance-access.lv2
    uri-map.lv2
'''.split()

lvtk_installer_lv2dir = '/Library/Audio/Plug-Ins/LV2'
lvtk_lv2_operations = '''<Operations>
    <Operation name="Mkdir">
        <Argument>@0@</Argument>
    </Operation>'''.format(lvtk_installer_lv2dir)

lvtk_lv2_operations += '''
    <Operation name="Extract">
        <Argument>@0@</Argument>
    </Operation>
'''.format('/Library/Audio/Plug-Ins')

lvtk_lv2_operations += '</Operations>'

foreach m : [ 'package.xml', 'ISC.txt']
    configure_file (input: m, output: m,
        install: true, install_dir: lvtk_deploy_lv2dir / 'meta',
        configuration: {
            'BUILDNUMBER': lvtk_build_number,
            'RELEASEDATE': lvtk_release_date,
            'OPERATIONS': lvtk_lv2_operations
        })
endforeach
