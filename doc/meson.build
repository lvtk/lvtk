dot = find_program ('dot', required : get_option('doc'))
doxygen = find_program ('doxygen', required : get_option('doc'))
sphinx_build = find_program ('sphinx-build', required : get_option('doc'))
build_docs = doxygen.found() and sphinx_build.found() and dot.found()

if build_docs

conf_py = configure_file (configuration: {
        'LVTK_VERSION' : meson.project_version() 
    },
    input : 'conf.py.in',
    output : 'conf.py')

overview = configure_file (configuration: {
        'LVTK_VERSION' : meson.project_version(),
        'LVTK_ABI_VERSION'  : lvtk_abi_version
    },
    input : 'overview.rst.in',
    output : 'overview.rst')

lvtk_rst_sources = files ('''
    development.rst
    dynmanifest.rst
    extensions.rst
    highlevel.rst
    index.rst
    styleguide.rst
    summary.rst
    plugins.rst
    projects.rst
    uis.rst
'''.split())

foreach f : lvtk_rst_sources
  fs.copyfile (f)
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

subdir ('html/_static/js')

endif
