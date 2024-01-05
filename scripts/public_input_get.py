"""Get public input for a request."""
import argparse
import json
import logging
import requests
import sys
from urllib.parse import urljoin

import constants
from auth_tools import get_headers


logger = logging.getLogger(__name__)


def get(key, url):
    headers = get_headers()
    url = urljoin(url, "/request/")
    if key:
        url += key
    res = requests.get(url=url, headers=headers, timeout=constants.REQUEST_TIMEOUT)
    if res.status_code != 200:
        logger.error(
            f"Get public input error for request {key}: {res.status_code} {res.text}"
        )
        return None
    return res.json()["input"]


def main():
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parser = argparse.ArgumentParser()
    parser.add_argument("-k", "--key", metavar="key", type=str, help="key of the request")
    parser.add_argument(
        "--url", action="store", default=constants.URL, help="url of a producer"
    )
    parser.add_argument(
        "-o", "--output", metavar="file", type=str, required=True, help="output file"
    )
    args = parser.parse_args()
    public_input = get(args.key, args.url)
    if public_input is None:
        sys.exit(1)
    with open(args.output, "w") as f:
        output = public_input.json()["input"]
        json.dump(output, f, indent=4)


if __name__ == "__main__":
    main()
