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

boost_dep = (dependency ('boost'))
