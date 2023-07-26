import pytest
import subprocess

STATEMENT_KEY='0000'
PROPOSAL_TEST_KEY= '123321'
REQUEST_PUBLIC_INPUT='example/input/arithmetic/input.json'

def test_push_request_with_public_input():
    completed_process = subprocess.run(['python3', 'scripts/request_tools.py', 'push', '--cost', '123','--key', STATEMENT_KEY, '--file', REQUEST_PUBLIC_INPUT])
    assert completed_process.returncode == 0

# def test_proposals_tools_push_with_public_input():
#     completed_process = subprocess.run(['python3', 'scripts/request_tools.py', 'push', '--cost', '123','--key', STATEMENT_KEY, '--file', 'file.json'])
#     assert completed_process.returncode == 0

def test_get_requests():
    completed_process = subprocess.run(['python3', 'scripts/request_tools.py', 'get'])
    assert completed_process.returncode == 0

def test_get_request_by_key():
    completed_process = subprocess.run(['python3', 'scripts/request_tools.py', 'get', '--key', PROPOSAL_TEST_KEY])
    assert completed_process.returncode == 0

def test_help():
    completed_process = subprocess.run(['python3', 'scripts/request_tools.py', '-h'])
    assert completed_process.returncode == 0