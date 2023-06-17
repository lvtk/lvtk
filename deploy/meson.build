lvtk_deploy_sdkdir = get_option('prefix') / 'packages' / 'org.lvtk.sdk'
lvtk_deploy_lv2dir = get_option('prefix') / 'packages' / 'org.lvtk.lv2'
lvtk_deploy_pluginsdir = get_option('prefix') / 'packages' / 'org.lvtk.plugins'
lvtk_deploy_targetdir = '/Library/Application Support/LVTK'
lvtk_deploy_platform  = ''

if host_machine.system() == 'windows'
    lvtk_deploy_platform = 'windows'
elif host_machine.system() == 'darwin'
    lvtk_deploy_platform = 'osx'
endif

configure_file (input: 'config.xml', output: 'config.xml',
    install: true, install_dir: 'config', configuration: {
        'VERSION': lvtk_version_with_build,
        'TARGETDIR': lvtk_deploy_targetdir,
        'PLATFORM': lvtk_deploy_platform })
configure_file (input: 'control.js', output: 'control.js',
    install: true, install_dir: 'config',
    configuration: {})

foreach pkg : 'lv2 sdk plugins'.split()
    subdir (pkg)
endforeach

if host_machine.system() == 'darwin'
    basename = 'lvtk-osx-@0@'.format (lvtk_version_with_build)

    extra_args = ''
    if get_option ('sign')
        codesign = find_program ('codesign', required : true)
        ident = get_option ('codesign-identity')
        extra_args = '-codesign="@0@" -hardened-runtime -sign-for-notarization="@0@" -timestamp'.format(ident)
    endif

    notary_script = ''
    if get_option('notarize') and get_option('sign')
        notary_script = '''xcrun altool --notarize-app --primary-bundle-id="org.lvtk.installer"  \
            -u @0@ -p @1@ --file @2@'''.format(
                get_option('apple-id'), 
                get_option ('app-password'),
                basename + '.dmg')
    endif

    macdeployqt = find_program ('macdeployqt', required : true)
    archivegen = find_program ('archivegen', required : true)
    binarycreator = find_program ('binarycreator', required : true)
    configure_file (input : 'installer.sh', output : 'installer.sh',
        install : true, install_dir : get_option('prefix'),
        configuration: {
            'BINARYCREATOR': binarycreator.full_path(),
            'CONFIGFILE': 'config/config.xml',
            'PACKAGES': 'packages',
            'INSTALLERBASE': basename,
            'MACDEPLOYQT': macdeployqt.full_path(),
            'MACDEPLOYQT_ARGS': extra_args,
            'NOTARYSCRIPT': notary_script,
            'ARCHIVEGEN': archivegen.full_path()
        })
elif host_machine.system() == 'win32'
    # placeholder
endif
