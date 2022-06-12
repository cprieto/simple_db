from unittest import TestCase
import sys
import unittest
from abc import ABC
from subprocess import run
from typing import NamedTuple


class CmdReturn(NamedTuple):
    output: list[str]
    retval: int


class BaseDbTests(ABC, unittest.TestCase):
    DBS = "dbs"

    def run_script(self, *args) -> list[str]:
        cmds = ''.join((x + '\n' for x in args))
        resp = run((DbTests.DBS,), capture_output=True, input=cmds.encode())

        return CmdReturn(resp.stdout.decode().split('\n'), resp.returncode)


class DbTests(BaseDbTests):
    def test_starts_and_exit(self):
        output, retval = self.run_script('.exit')
        
        self.assertEqual(['db > '], output)
        self.assertEqual(retval, 0)

    def test_inserts_and_retrieves_a_row(self):
        cmds = ['insert 1 user1 person1@example.com', 'select', '.exit']
        output, retval = self.run_script(*cmds)

        self.assertEqual(output, ['db > Executed.', 'db > (1, user1, person1@example.com)', 'Executed.', 'db > '])
        self.assertEqual(retval, 0)
        

if __name__ == '__main__':
    if len(sys.argv) > 1:
        BaseDbTests.DBS = sys.argv.pop()

    unittest.main()
