doxygen = find_program ('doxygen', required : get_option('docs'))
sphinx_build = find_program ('sphinx-build', required : get_option('docs'))

if doxygen.found() and sphinx_build.found()

conf_py = configure_file (configuration: {
        'LVTK_VERSION' : meson.project_version() 
    },
    input : 'conf.py.in',
    output : 'conf.py')

conf_py = configure_file (configuration: {
        'LVTK_VERSION' : meson.project_version(),
        'LVTK_ABI_VERSION'  : lvtk_abi_version
    },
    input : 'overview.rst.in',
    output : 'overview.rst')

    


lvtk_rst_sources = files ('''
    deployment.rst
    dynmanifest.rst
    extensions.rst
    highlevel.rst
    hosting.rst
    index.rst
    summary.rst
    widgets.rst
    plugins.rst
    projects.rst
    uis.rst
'''.split())

foreach f : lvtk_rst_sources
  configure_file(copy: true, input: f, output: '@PLAINNAME@')
endforeach

subdir ('api')
subdir ('ui')

docs = custom_target(
  'html',
  command: [sphinx_build, '-M', 'html', '@OUTDIR@', '@OUTDIR@',
            '-E', '-q', '-t', 'html'],
  input: [ lvtk_rst_sources ],
  output: 'html',
  build_by_default: true,
  install: true,
  install_dir: get_option('datadir') / 'doc' / 'lvtk-3')

subdir ('html/_static/js')

endif
