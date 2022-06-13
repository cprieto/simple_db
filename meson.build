project('dbstuff', 'c', version: '1.0', default_options: ['c_std=c11'])

subdir('src')
if meson.get_compiler('c').get_id() == 'gcc'
    add_project_arguments('-D_GNU_SOURCE', language: 'c')
endif

dbs = executable('dbs', db_files)

# Tests depend on a created executable
subdir('tests')
