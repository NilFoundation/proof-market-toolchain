import pytest
import subprocess

def test_signin():
    completed_process = subprocess.run(['python3', 'scripts/auth_tools.py'])
    assert completed_process.returncode == 0