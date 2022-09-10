doxygen = find_program ('doxygen', required : false)
dot = find_program ('dot', required : false)
cppunit_dep = dependency ('cppunit', required : false)

cc = meson.get_compiler ('c')
dl_dep = cc.find_library ('dl', required: false)
threads_dep = dependency('threads')

lv2_dep = dependency ('lv2',
    fallback : [ 'lv2', 'lv2_dep' ],
    version : '>= 1.18.0', 
    required : true)

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

boost_dep = (dependency ('boost'))

evg_dep = dependency ('evg-0', 
    fallback : [ 'evg', 'evg_dep' ],
    required : false)

xcb_dep = dependency ('x11-xcb',
    version: '>= 1.6.9', required: true)
