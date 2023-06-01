import os
import argparse
import json
from constants import URL
import requests


def create_credentials_file(file_name: str, value: str):
    with open(os.path.dirname(os.path.abspath(__file__)) + f"/.{file_name}", "w") as f:
        f.write(value)


def read_credentials_file(file_name: str) -> str:
    credentials_file_path = (
        os.path.dirname(os.path.abspath(__file__)) + f"/.{file_name}"
    )

    if not os.path.exists(credentials_file_path):
        return None

    return open(credentials_file_path, "r").read().strip("\n")


secret = read_credentials_file("secret")
user = read_credentials_file("user")


def update_auth(auth):

    url = URL + "_open/auth"
    body = {"username": user, "password": secret}

    response = requests.post(url, json=body)
    if response.status_code != 200:
        print(f"Update auth error: {response.status_code} {response.text}")
    else:
        with open(auth, "w") as f:
            headers = {"Authorization": f'Bearer {response.json()["jwt"]}'}
            json.dump(headers, f)
    return response


def get_headers(auth):
    headers = {}
    if auth is None:
        auth = "auth.json"
        response = update_auth(auth)
        if response.status_code != 200:
            return
    with open(auth, "r") as f:
        auth_data = json.load(f)
        headers.update(auth_data)
    return headers


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-a",
        "--auth",
        action="store",
        default="auth.json",
        help="file to store jwt token",
    )
    args = parser.parse_args()

    update_auth(args.auth)
