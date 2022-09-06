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

lua_dep = dependency ('lua-5.4', 
    fallback : [ 'lua', 'lua_dep' ],
    version : '>= 5.4.4',
    default_options : [
        'interpreter=true',
        'default_library=static'
    ],
    required : true
)

sol2_dep = dependency ('sol2', 
    fallback : [ 'sol2', 'sol2_dep' ],
    default_options: [],
    required : true
)
