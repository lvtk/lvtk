lvtk_docs_js_sources = files ('''
    custom.js
'''.split())

foreach f : lvtk_docs_js_sources
  configure_file (copy: true, input: f, output: '@PLAINNAME@')
endforeach
