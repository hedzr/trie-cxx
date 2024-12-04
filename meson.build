# todo, wip

project('ask1x-cxx', ['cpp'],
  version         : run_command(find_program('cmake/versions-extract.py'), check: true).stdout().strip(),
  default_options : ['cpp_std=c++17', 'warning_level=3']
)

cxx = meson.get_compiler('cpp')

ASK_CXX_inc = include_directories(['include'])

ASK_CXX_dep = declare_dependency(
  include_directories : ASK_CXX_inc,
  version             : meson.project_version(),
)

if get_option('tests')
    warnings = ['-Wshadow', '-Wsign-conversion', '-Wswitch-enum']
    if cxx.get_id() == 'gcc' and cxx.version().version_compare('>=4.9')
        warnings += '-Weffc++'
    endif
    add_project_arguments(cxx.get_supported_arguments(warnings), language: 'cpp')

    subdir('test')
endif