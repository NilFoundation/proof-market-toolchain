import pytest
import subprocess

STATEMENT_KEY='0000'
PROPOSAL_TEST_KEY= '123321'

def test_push_proposal_without_public_input():
    completed_process = subprocess.run(['python3', 'scripts/proposal_tools.py', 'push', '--cost', '123','--key', STATEMENT_KEY])
    assert completed_process.returncode == 0

# def test_proposals_tools_push_with_public_input():
#     completed_process = subprocess.run(['python3', 'scripts/proposal_tools.py', 'push', '--cost', '123','--key', STATEMENT_KEY, '--file', 'file.json'])
#     assert completed_process.returncode == 0

def test_get_proposal():
    completed_process = subprocess.run(['python3', 'scripts/proposal_tools.py', 'get'])
    assert completed_process.returncode == 0

def test_help():
    completed_process = subprocess.run(['python3', 'scripts/proposal_tools.py', '-h'])
    assert completed_process.returncode == 0

def test_get_proposal_by_key():
    completed_process = subprocess.run(['python3', 'scripts/proposal_tools.py', 'get', '--key', PROPOSAL_TEST_KEY])
    assert completed_process.returncode == 0