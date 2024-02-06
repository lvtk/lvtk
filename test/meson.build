
## REUSE test
if reuse_program.found()
  test('REUSE', reuse_program, 
    args: [ '--root', meson.project_source_root(), 'lint' ], 
    suite: 'metadata')
endif

if boost_dep.found()

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
    ui_font_test.cpp
    ui_path_test.cpp
    weak_ref_test.cpp
    main_unit.cpp
'''.split()

if lilv_dep.found()
    lvtk_unit_test_sources += 'host_world_test.cpp'
endif

unit = executable ('unit',
    lvtk_unit_test_sources,
    dependencies : [ boost_dep, lvtk_internal_dep, lvtk_widgets_dep, lilv_dep ],
    gnu_symbol_visibility : 'hidden',
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
    
    Font
    Observer
    String
    Path
    WeakRef
'''.split()

if lilv_dep.found()
    lvtk_unit_tests += 'World'
endif

foreach t : lvtk_unit_tests
    test (t, unit, args : [ '-t', t ],
        env : [
            'LV2_PATH=@0@'.format (meson.project_build_root())
        ]
    )
endforeach

if not get_option('bindings').disabled()
    # subdir ('lua')
endif

endif
