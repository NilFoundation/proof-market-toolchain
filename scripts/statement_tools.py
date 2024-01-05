import argparse
import json
import logging
import requests
import sys
from urllib.parse import urljoin

import constants
from auth_tools import get_headers


logger = logging.getLogger(__name__)


def push_statement(file, url):
    if file:
        f = open(file, "r")
        data = json.load(f)
    else:
        logger.error("Error: file is required")
        return None

    headers = get_headers(url=url)
    url = urljoin(url, "/statement")
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logger.error(f"Error: {res.status_code} {res.text}")
        return None
    else:
        logger.info(f"Statement from {file} was pushed")
        return res


def list_statements(url) -> list[dict]:
    headers = get_headers(url=url)
    url = urljoin(url, "/statement")
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logger.error(f"Error: {res.status_code} {res.text}")
        return None
    else:
        logger.debug("Statements:\n%s", json.dumps(res.json(), indent=2))
        return res.json()


def get_statements(url, key=None, output=None):
    headers = get_headers(url=url)
    url = urljoin(url, "/statement/")
    if key is not None:
        url = urljoin(url, key)
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logger.error(f"Error: {res.status_code} {res.text}")
        return None
    return res.json()


def push_parser(args):
    return push_statement(args.file, args.url) is not None


def get_parser(args):
    statements = get_statements(args.url, args.key, args.output) is not None
    if args.output:
        with open(args.output, "w") as f:
            json.dump(statements, f, indent=2)
            logger.info("Statements are saved into %s", args.output)
    else:
        print(f"Statements:\n{json.dumps(statements, indent=2)}")



def main():
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument(
        "--url", action="store", default=constants.URL, help="url of a producer"
    )

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(help="sub-command help")

    parser_push = subparsers.add_parser("push", help="push statement", parents=[parent_parser])
    parser_push.set_defaults(func=push_parser)
    parser_push.add_argument("--file", type=str, required=True, help="file")

    parser_get = subparsers.add_parser("get", help="get statement", parents=[parent_parser])
    parser_get.set_defaults(func=get_parser)
    parser_get.add_argument("--key", type=str, help="statement key")
    parser_get.add_argument("-o", "--output", type=str, help="output file")

    args = parser.parse_args()
    if not hasattr(args, 'func'):
        # invalid subparser
        parser.print_help()
        sys.exit(1)

    sys.exit(0 if args.func(args) else 1)


if __name__ == "__main__":
    main()
