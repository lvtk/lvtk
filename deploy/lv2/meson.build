
foreach m : [ 'package.xml', 'ISC.txt']
    configure_file (input: m, output: m,
        install: true, install_dir: lvtk_deploy_lv2dir_osx / 'meta',
        configuration: {
        'VERSION': lvtk_version_with_build,
        'RELEASEDATE': lvtk_release_date
    })
endforeach
