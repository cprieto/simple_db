from unittest import TestCase
import sys
import unittest
from abc import ABC
from subprocess import check_output


class BaseDbTests(ABC, unittest.TestCase):
    DBS = "dbs"

    @staticmethod
    def run_script(*args) -> list[str]:
        cmds = ''.join((x + '\n' for x in args))
        resp = check_output((DbTests.DBS,), input=cmds.encode())

        return resp.decode().split('\n')


class DbTests(BaseDbTests):
    def test_starts_and_exit(self):
        output = DbTests.run_script('.exit')
        
        self.assertEqual(['db > '], output)

    def test_inserts_and_retrieves_a_row(self):
        cmds = ['insert 1 user1 person1@example.com', 'select', '.exit']
        output = DbTests.run_script(*cmds)

        self.assertEqual(output, ['db > Executed.', 'db > (1, user1, person1@example.com)', 'Executed.', 'db > '])

    def test_prints_error_message_when_table_is_full(self):
        cmds = [f'insert {x} user{x} user{x}@sample.com' for x in range(0, 1401)]
        cmds += ['.exit']

        output = DbTests.run_script(*cmds)
        self.assertEqual(output[-2], 'db > Error: Table full.')

    def test_it_can_insert_fields_with_max_chars(self):
        username = 'a'*32
        cmds = [f'insert 1 {username} foo@bar.com', 'select', '.exit']

        output = DbTests.run_script(*cmds)
        self.assertEqual(output, ['db > Executed.', f'db > (1, {username}, foo@bar.com)', 'Executed.', 'db > '])

    def test_it_prints_error_message_if_string_is_too_long(self):
        username = 'a'*33
        cmds = [f'insert 1 {username} foo@bar.com', '.exit']

        output = DbTests.run_script(*cmds)
        self.assertEqual(output, ['db > String is too long.', 'db > '])

    def test_it_prints_an_error_message_if_id_is_negative(self):
        cmds = [f'insert -1 foo foo@bar.com', '.exit']

        output = DbTests.run_script(*cmds)
        self.assertEqual(output, ['db > ID must be positive.', 'db > '])


if __name__ == '__main__':
    if len(sys.argv) > 1:
        BaseDbTests.DBS = sys.argv.pop()

    unittest.main()
