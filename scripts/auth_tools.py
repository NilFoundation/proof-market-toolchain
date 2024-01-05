import argparse
import json
import logging
import os
import requests
import stat
from pathlib import Path
from urllib.parse import urljoin

import constants


logger = logging.getLogger(__name__)


def _warn_if_wide_permissions(path):
    file_stat = os.stat(path)
    mode = file_stat.st_mode
    if mode & stat.S_IRWXG or mode & stat.S_IRWXO:
        logger.warning(f"File {path} has too wide permissions: {oct(mode & 0o777)}")


class Authenticator:
    USER_ENV = "NIL_USER"
    SECRET_ENV = "NIL_SECRET"
    STORAGE_DIR = Path.home().joinpath(".config", "proof-market")
    CREDENTIALS_BASENAME = "credentials.json"
    AUTH_BASENAME = "auth.json"

    class Helpers:
        @staticmethod
        def get_base_dir(directory):
            return Authenticator.STORAGE_DIR if directory is None else Path(directory)

        @staticmethod
        def get_auth_file_path(directory):
            return Authenticator.Helpers.get_base_dir(directory) / Authenticator.AUTH_BASENAME

        @staticmethod
        def get_credentials_file_path(directory):
            return Authenticator.Helpers.get_base_dir(directory) / Authenticator.CREDENTIALS_BASENAME

    def __init__(self, url: str, directory: str | None = None):
        self.credentials_path = Authenticator.Helpers.get_credentials_file_path(directory)
        self.auth_path = Authenticator.Helpers.get_auth_file_path(directory)
        self.username, self.secret = self.get_credentials()
        self.url = url

    def get_credentials(self) -> tuple[str, str]:
        username = os.environ.get(self.USER_ENV)
        secret = os.environ.get(self.SECRET_ENV)

        if username is None or secret is None:
            logger.info(f"Environment variables not set. Reading credentials from {self.credentials_path}.")
            _warn_if_wide_permissions(self.credentials_path)
            with open(self.credentials_path, 'r') as file:
                data = json.load(file)
            username = data.get('username')
            secret = data.get('secret')
            if not username or not secret:
                raise ValueError("Credentials not found in JSON file.")
        return username, secret

    def update_auth_file(self):
        url = urljoin(self.url, "/user/signin")
        body = {"username": self.username, "password": self.secret}

        response = requests.post(url, json=body)
        if response.status_code != 200:
            raise RuntimeError(f"Failed to fetch auth: {response.status_code} {response.text}")
        with open(self.auth_path, "w", opener=lambda path, flags: os.open(path, flags, 0o600)) as f:
            headers = {"Authorization": f"Bearer {response.json()['jwt']}"}
            json.dump(headers, f)
        logger.info("Auth file updated.")

    @staticmethod
    def create_credentials_file(username: str, secret: str, directory: str | None = None):
        credentials_data = {
            "username": username,
            "secret": secret,
        }
        credentials_json = json.dumps(credentials_data)
        credentials_path = Authenticator.Helpers.get_credentials_file_path(directory)
        os.makedirs(os.path.dirname(credentials_path), exist_ok=True)
        with open(credentials_path, "w", opener=lambda path, flags: os.open(path, flags, 0o600)) as f:
            f.write(credentials_json)
        logger.info(f"Credentials saved to {credentials_path}")


def get_headers(directory=None, url=None):
    auth_path = Authenticator.Helpers.get_auth_file_path(directory)
    if not os.path.exists(auth_path):
        logger.info(f"Auth file {auth_path} does not exist.")
        if url is not None:
            logger.info(f"Trying to create it with request to {url}")
            authenticator = Authenticator(url)
            authenticator.update_auth_file()
        else:
            raise RuntimeError("No way to fetch auth.json")

    _warn_if_wide_permissions(auth_path)
    with open(auth_path, "r") as f:
        auth_data = json.load(f)
    return auth_data


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-u",
        "--url",
        action="store",
        default=constants.URL,
        help="URL of proof market API",
    )
    parser.add_argument(
        "-d",
        "--dir",
        action="store",
        default=constants.URL,
        help="Directory to store credentials and auth file",
    )
    args = parser.parse_args()

    Authenticator(args.url, args.dir).update_auth_path()
    logger.info(f"Auth file at {args.dir} updated")


if __name__ == "__main__":
    main()
