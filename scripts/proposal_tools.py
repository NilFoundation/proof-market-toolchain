"""Proposal get and push functionality"""
import argparse
import json
import logging
import requests
import sys
from urllib.parse import urljoin

import constants
from auth_tools import get_headers


def push(url, key, cost):
    data = {
        "statement_key": key,
        "cost": cost,
    }
    headers = get_headers()
    url = urljoin(url, "/proposal/")
    res = requests.post(url=url, json=data, headers=headers, timeout=constants.REQUEST_TIMEOUT)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return None
    else:
        return res.json()


def get(url, status=None):
    headers = get_headers()
    url = urljoin(url, "/proposal/")
    if status is not None:
        url = urljoin(url, '?status={status}')
    res = requests.get(url=url, headers=headers, timeout=constants.REQUEST_TIMEOUT)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return None
    else:
        logging.debug("Proposals:\n %s", json.dumps(res.json(), indent=4))
        return res.json()


def push_parser(push_args):
    res = push(push_args.url, push_args.key, push_args.cost)
    return res is not None


def get_parser(get_args):
    return get(get_args.url, get_args.key) is not None


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument(
        "--url", action="store", default=constants.URL, help="url of a producer"
    )

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(help="sub-command help")
    parser_push = subparsers.add_parser("push", help="push proposal", parents=[parent_parser])
    parser_push.set_defaults(func=push_parser)
    parser_get = subparsers.add_parser("get", help="get proposal", parents=[parent_parser])
    parser_get.set_defaults(func=get_parser)
    parser_push.add_argument("--cost", type=float, required=True, help="cost")
    parser_push.add_argument(
        "--key", type=str, required=True, help="key of the statement"
    )
    parser_get.add_argument("--key", type=str, help="key of the proposal")
    args = parser.parse_args()
    if not hasattr(args, 'func'):
        # invalid subparser
        parser.print_help()
        sys.exit(1)

    sys.exit(0 if args.func(args) else 1)
