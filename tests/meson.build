lvtk_test_sources = '''
    atom_test.cpp
    descriptor_test.cpp
    main.cpp
    urid_test.cpp
    bufsize_test.cpp
    dynmanifest_test.cpp
    options_test.cpp
    log_test.cpp
    worker_test.cpp
    data_access_test.cpp
    instance_access_test.cpp
    state_test.cpp
    weak_ref_test.cpp
    ui_path_test.cpp
    ../lvtk.lv2/volume.cpp
'''.split()

test ('cppunit', executable (
    'cppunit',
    lvtk_test_sources,
    cpp_args : [ '-DLVTK_NO_SYMBOL_EXPORT' ],
    include_directories : ['.'],
    dependencies : [ cppunit_dep, lvtk_dep ],
    install : false)
)

lvtk_test_sources = '''
    string_test.cpp
    unit_tests.cpp
'''.split()

unit = executable('unit',
    lvtk_test_sources,
    dependencies : [ boost_dep, lvtk_internal_dep ])

test ('strings', unit, args : [ '-t', 'strings' ])
