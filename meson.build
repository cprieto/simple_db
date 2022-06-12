project('dbstuff', 'c', version: '1.0', default_options: ['c_std=c11'])

subdir('src')

dbs = executable('dbs', db_files)

# Tests depend on a created executable
subdir('tests')
