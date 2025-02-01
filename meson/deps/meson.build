cc = meson.get_compiler ('c')
cpp = meson.get_compiler ('cpp')
dl_dep = cc.find_library ('dl', required: false)
reuse_program = find_program ('reuse', required : false)
threads_dep = dependency ('threads')

### Lv2 Core
lv2_dep = dependency ('lv2',
    fallback : [ 'lv2', 'lv2_dep' ],
    version : '>= 1.16.0', 
    required : true,
    default_options : [
        'tests=disabled',
        'plugins=disabled',
        'docs=disabled'
    ])

lvtk_lv2dir = get_option('lv2dir')
if lvtk_lv2dir == ''
  if target_machine.system() == 'darwin'
    lvtk_lv2dir = '/Library/Audio/Plug-Ins/LV2'
  elif target_machine.system() == 'haiku'
    lvtk_lv2dir = '/boot/common/add-ons/lv2'
  elif target_machine.system() == 'windows'
    lvtk_lv2dir = 'C:/Program Files/Common Files/LV2'
  else
    lvtk_lv2dir = get_option('prefix') / get_option('libdir') / 'lv2'
  endif
endif

### Boost
lvtk_build_widgets = true
boost_dep = dependency ('boost', required: get_option('test').enabled())
