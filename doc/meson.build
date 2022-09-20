doxygen = find_program ('doxygen', required : get_option('docs'))
dox_to_sphinx = find_program ('../scripts/dox_to_sphinx.py', required : false)
sphinx_build = find_program ('sphinx-build', required : get_option('docs'))

if doxygen.found() and sphinx_build.found()

conf_py = configure_file (configuration: {
        'LVTK_VERSION' : meson.project_version() 
    },
    input : 'conf.py.in',
    output : 'conf.py')

lvtk_rst_sources = files ('''
    deployment.rst
    event-loop.rst
    events.rst
    index.rst
    overview.rst
    summary.rst
    view.rst
    world.rst
'''.split())

foreach f : lvtk_rst_sources
  configure_file(copy: true, input: f, output: '@PLAINNAME@')
endforeach

subdir ('api')

docs = custom_target(
  'html',
  command: [sphinx_build, '-M', 'html', '@OUTDIR@', '@OUTDIR@',
            '-E', '-q', '-t', 'html'],
  input: [ lvtk_rst_sources ],
  output: 'html',
  build_by_default: true,
  install: true,
  install_dir: get_option('datadir') / 'doc' / 'lvtk-3')

endif
