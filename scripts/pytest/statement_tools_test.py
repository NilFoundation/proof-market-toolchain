import pytest
import subprocess

STATEMENT_KEY='0000'

def test_get_statement_by_key():
    completed_process = subprocess.run(['python3', 'scripts/statement_tools.py', 'get','--key', STATEMENT_KEY])
    assert completed_process.returncode == 0
