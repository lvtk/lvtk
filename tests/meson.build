lvtk_test_sources = '''
    atom_test.cpp
    descriptor_test.cpp
    main.cpp
    urid_test.cpp
    bufsize_test.cpp
    dynmanifest_test.cpp
    options_test.cpp
    worker_test.cpp
    data_access_test.cpp
    instance_access_test.cpp
    state_test.cpp
    ../plugins/volume.cpp
'''.split()

lvtk_test_deps = [ cppunit_dep ]

lvtk_test_app = executable ('test_lvtk', lvtk_test_sources,
    cpp_args : [ '-DLVTK_NO_SYMBOL_EXPORT' ],
    include_directories : ['.', '..'],
    dependencies : lvtk_test_deps)

test ('cppunit', lvtk_test_app)
