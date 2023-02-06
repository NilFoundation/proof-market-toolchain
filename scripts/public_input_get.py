"""Get public input for a bid."""
import requests
import sys
import logging
import argparse
import json
from constants import DB_NAME, URL, MOUNT, REQUEST_TIMEOUT
from auth_tools import get_headers


def get(key, auth):
    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/bid/"
    if key:
        url += key
    res = requests.get(url=url, headers=headers, timeout=REQUEST_TIMEOUT)
    if res.status_code != 200:
        logging.error(
            f"Get public input error for bid {key}: {res.status_code} {res.text}"
        )
        sys.exit(1)
    else:
        return res


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parser = argparse.ArgumentParser()
    parser.add_argument("-k", "--key", metavar="key", type=str, help="key of the bid")
    parser.add_argument("--auth", metavar="auth", type=str, help="auth")
    parser.add_argument(
        "-o", "--output", metavar="file", type=str, required=True, help="output file"
    )
    args = parser.parse_args()
    public_input = get(args.key, args.auth)
    with open(args.output, "w") as f:
        output = public_input.json()
        json.dump(output, f, indent=4)
