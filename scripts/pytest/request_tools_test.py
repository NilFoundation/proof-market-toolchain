import pytest
import subprocess
import json
from subprocess import Popen, PIPE

STATEMENT_KEY='0000'
PROPOSAL_TEST_KEY= '123321'
REQUEST_PUBLIC_INPUT='example/input/arithmetic/input.json'

PROVED_REQUEST_STATEMENT_KEY='0000'
PROVED_REQUEST_PUBLIC_INPUT='example/input/arithmetic/input.json'

def test_push_request_with_public_input():
    completed_process = subprocess.run(['python3', 'scripts/request_tools.py', 'push', '--cost', '123','--key', STATEMENT_KEY, '--file', REQUEST_PUBLIC_INPUT])
    assert completed_process.returncode == 0


def test_test_already_proved_request():
    request_proof_output = subprocess.run(['python3', 'scripts/request_tools.py', 'push', '--cost', '123','--key', PROVED_REQUEST_STATEMENT_KEY, '--file', PROVED_REQUEST_PUBLIC_INPUT],stdin=PIPE, stdout=PIPE, stderr=PIPE)
    reequest_proof_output_json = json.loads(request_proof_output.stdout)
    request_key=reequest_proof_output_json["_key"]
    request_by_key_output = subprocess.run(['python3', 'scripts/request_tools.py', 'get', '--key', request_key],stdout=subprocess.PIPE)
    request_by_key_output_json = json.loads(request_by_key_output.stdout)
    assert request_by_key_output_json["status"]=="completed"


def test_get_requests():
    completed_process = subprocess.run(['python3', 'scripts/request_tools.py', 'get'])
    assert completed_process.returncode == 0

def test_get_request_by_key():
    completed_process = subprocess.run(['python3', 'scripts/request_tools.py', 'get', '--key', PROPOSAL_TEST_KEY])
    assert completed_process.returncode == 0

def test_help():
    completed_process = subprocess.run(['python3', 'scripts/request_tools.py', '-h'])
    assert completed_process.returncode == 0