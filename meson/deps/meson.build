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

pugl_dep = dependency ('pugl-0', 
    fallback : [ 'pugl', 'pugl_dep' ],
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
