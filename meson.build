project('ranae', ['cpp'], version : '0.0.1', meson_version : '>= 0.49', default_options : [
  'warning_level=2',
  'c_std=c11',
  'cpp_std=c++20'
])

ranae_compiler = meson.get_compiler('cpp')
add_project_arguments(ranae_compiler.get_supported_arguments([
  '-Wno-missing-field-initializers',
]), language : 'cpp')

ranae_include = include_directories(['include'])
sdl2_dep = dependency('SDL2')
vulkan_dep = dependency('vulkan') # get rid of me!

subdir('src')