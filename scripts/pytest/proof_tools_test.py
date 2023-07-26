import pytest
import subprocess

REQUEST_TEST_KEY='0000'
PROPOSAL_TEST_KEY= '123321'
REQUEST_PUBLIC_INPUT='example/input/arithmetic/input.json'

def test_get_proof_by_proposal_key():
    completed_process = subprocess.run(['python3', 'scripts/proof_tools.py','get', '--request_key', REQUEST_TEST_KEY])
    assert completed_process.returncode == 0

def test_push_proof_by_request_key():
    completed_process = subprocess.run(['python3', 'scripts/proof_tools.py','push','--request_key', REQUEST_TEST_KEY, '--file','example/proofs/statements/32292.txt'])
    assert completed_process.returncode == 0

def test_push_proof_by_proposal_key():
    completed_process = subprocess.run(['python3', 'scripts/proof_tools.py','push','--proposal_key', REQUEST_TEST_KEY, '--file','example/proofs/statements/32292.txt'])
    assert completed_process.returncode == 0