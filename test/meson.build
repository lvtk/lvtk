
## Unit Tests
lvtk_unit_test_sources = '''
    atom_test.cpp
    bufsize_test.cpp
    data_access_test.cpp
    descriptor_test.cpp
    dynmanifest_test.cpp
    instance_access_test.cpp
    log_test.cpp
    options_test.cpp
    state_test.cpp
    urid_test.cpp
    worker_test.cpp

    observer_test.cpp
    string_test.cpp
    ui_path_test.cpp
    weak_ref_test.cpp

    main_unit.cpp
'''.split()

unit = executable ('unit',
    lvtk_unit_test_sources,
    dependencies : [ boost_dep, lvtk_internal_dep ],
    cpp_args : ['-DLVTK_NO_SYMBOL_EXPORT'])

lvtk_unit_tests = '''
    Atom
    BufSize
    DataAccess
    Descriptor
    DynManifest
    InstanceAccess
    Log
    Options
    State
    URID
    Worker
    
    Observer
    String
    Path
    WeakRef
'''.split()
foreach t : lvtk_unit_tests
    test (t, unit, args : [ '-t', t ])
endforeach
