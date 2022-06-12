py_exe = find_program('python3', 'python')
test_files = files('test_db.py')

# simply run python test_db.py dbs_executable
test('all_tests', py_exe, args: [test_files, dbs.full_path()])