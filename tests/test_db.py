import sys
import unittest
from abc import ABC
from os import remove
from uuid import uuid4
from pathlib import Path
from subprocess import check_output


class BaseDbTests(ABC, unittest.TestCase):
    DBS = "dbs"

    def run_script(self, *args) -> list[str]:
        cmds = ''.join((x + '\n' for x in args))
        resp = check_output((DbTests.DBS, self.filename), input=cmds.encode())

        return resp.decode().split('\n')

    def setUp(self) -> None:
        self.filename = f'{str(uuid4())}.db'

    def tearDown(self) -> None:
        path = Path(self.filename)
        if path.is_file():
            remove(self.filename)


class DbTests(BaseDbTests):
    def test_starts_and_exit(self):
        output = self.run_script('.exit')
        
        self.assertEqual(['db > '], output)

    def test_inserts_and_retrieves_a_row(self):
        cmds = ['insert 1 user1 person1@example.com', 'select', '.exit']
        output = self.run_script(*cmds)

        self.assertEqual(output, ['db > Executed.', 'db > (1, user1, person1@example.com)', 'Executed.', 'db > '])

    def test_prints_error_message_when_table_is_full(self):
        cmds = [f'insert {x} user{x} user{x}@sample.com' for x in range(0, 1401)]
        cmds += ['.exit']

        output = self.run_script(*cmds)
        self.assertEqual(output[-2], 'db > Error: Table full.')

    def test_it_can_insert_fields_with_max_chars(self):
        username = 'a'*32
        cmds = [f'insert 1 {username} foo@bar.com', 'select', '.exit']

        output = self.run_script(*cmds)
        self.assertEqual(output, ['db > Executed.', f'db > (1, {username}, foo@bar.com)', 'Executed.', 'db > '])

    def test_it_prints_error_message_if_string_is_too_long(self):
        username = 'a'*33
        cmds = [f'insert 1 {username} foo@bar.com', '.exit']

        output = self.run_script(*cmds)
        self.assertEqual(output, ['db > String is too long.', 'db > '])

    def test_it_prints_an_error_message_if_id_is_negative(self):
        cmds = [f'insert -1 foo foo@bar.com', '.exit']

        output = self.run_script(*cmds)
        self.assertEqual(output, ['db > ID must be positive.', 'db > '])

    def test_it_keeps_data_after_closing_connection(self):
        cmds = ['insert 1 user1 person1@example.com', '.exit']
        output = self.run_script(*cmds)
        self.assertEqual(output, ['db > Executed.', 'db > '])

        cmds = ['select', '.exit']
        output = self.run_script(*cmds)
        self.assertEqual(output, ['db > (1, user1, person1@example.com)', 'Executed.', 'db > '])


if __name__ == '__main__':
    if len(sys.argv) > 1:
        BaseDbTests.DBS = sys.argv.pop()

    unittest.main()
