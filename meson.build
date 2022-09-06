project ('lvtk', ['cpp', 'c'], version : '3.0.0',
    meson_version : '>= 0.53.0',
    default_options : [ 'cpp_std=c++14' ])

doxygen = find_program ('doxygen')
dot = find_program ('dot')
cppunit_dep = dependency ('cppunit', required : false)

lv2_dep = dependency ('lv2', version : '>= 1.15.4', required : false)

pugl_opts = [
    'tests=disabled',
    'examples=disabled',
    'cairo=disabled',
    'vulkan=disabled',
    'stub=false',
    # 'default_library=static'
]

puglpp_dep = dependency ('puglpp-0', 
    fallback : [ 'pugl', 'puglpp_dep' ],
    version : '>= 0.4.0',
    required : true,
    default_options : pugl_opts
)

pugl_gl_dep = dependency ('pugl-gl-0', 
    fallback : [ 'pugl', 'gl_backend_dep' ],
    version : '>= 0.4.0',
    default_options : pugl_opts,
    required : true
)

lvtk_dep = declare_dependency (
    include_directories : [ 'include', 'src' ],
    dependencies : [ lv2_dep ])

subdir ('src')
subdir ('lvtk.lv2')
subdir ('demo')

install_subdir ('include' / 'lvtk', install_dir : 'include' / 'lvtk-3')

pkg = import ('pkgconfig')
pkg.generate (
    name        : 'LVTK',
    filebase    : 'lvtk-3',
    version     : meson.project_version(),
    description : 'LV2 C++ templates',
    subdirs     : 'lvtk-3',
    requires    : [])
pkg.generate (liblvtk_ui,
    name        : 'LVTK',
    filebase    : 'lvtk-ui-3',
    version     : meson.project_version(),
    description : 'LV2 UI Library',
    subdirs     : 'lvtk-3',
    requires    : [])
pkg.generate (liblvtk_gl,
    name        : 'LVTK',
    filebase    : 'lvtk-gl-3',
    version     : meson.project_version(),
    description : 'LVTK OpenGL Backend Library',
    subdirs     : 'lvtk-3',
    requires    : [])

if doxygen.found() and dot.found()
    subdir ('doc')
endif

if cppunit_dep.found()
    subdir ('tests')
endif
