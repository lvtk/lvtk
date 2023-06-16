
lvtk_deploy_sdkdir_meta = lvtk_deploy_sdkdir / 'meta'
configure_file (input: 'package.xml', output: 'package.xml',
    install: true, install_dir: lvtk_deploy_sdkdir_meta,
    configuration: {
        'VERSION': lvtk_version_with_build,
        'RELEASEDATE': lvtk_release_date
    })
