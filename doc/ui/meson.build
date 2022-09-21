lvtk_ui_rst_sources = files ('''
    event-loop.rst
    events.rst
    main.rst
    widget.rst
'''.split())

foreach f : lvtk_ui_rst_sources
  configure_file(copy: true, input: f, output: '@PLAINNAME@')
endforeach
