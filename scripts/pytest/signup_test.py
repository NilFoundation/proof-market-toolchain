import pytest
import subprocess

USER='test_user'
PASSWORD='test_passowrd'
EMAIL= 'test@test.test'

def test_create_new_user():
    completed_process = subprocess.run(['python3', 'scripts/signup.py', 'user','-u', USER, '-p', PASSWORD,  '-e', EMAIL])
    assert completed_process.returncode == 0

def test_register_user_as_producer():
    completed_process = subprocess.run(['python3', 'scripts/signup.py', 'producer','-u', USER])
    assert completed_process.returncode == 0