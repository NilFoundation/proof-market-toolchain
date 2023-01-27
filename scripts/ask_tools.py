"""Ask get and push functionality"""
import requests
import json
import logging
import argparse
from constants import DB_NAME, URL, MOUNT, REQUEST_TIMEOUT
from auth_tools import get_headers


def push(auth, key, cost):
    data = {
        "statement_key": key,
        "cost": cost,
    }
    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/ask/"
    res = requests.post(url=url, json=data, headers=headers, timeout=REQUEST_TIMEOUT)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Limit ask:\t {res.json()}")
        return res.json()


def get(auth, key):
    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/ask/"
    if key:
        url += key
    else:
        url += "?limit=100"
    res = requests.get(url=url, headers=headers, timeout=REQUEST_TIMEOUT)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Ask:\n {json.dumps(res.json(), indent=4)}")
        return res.json()


def push_parser(push_args):
    return push(push_args.auth, push_args.key, push_args.cost)


def get_parser(get_args):
    return get(get_args.auth, get_args.key)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parser = argparse.ArgumentParser()
    parser.add_argument("--auth", type=str, help="auth file")
    subparsers = parser.add_subparsers(help="sub-command help")
    parser_push = subparsers.add_parser("push", help="push ask")
    parser_push.set_defaults(func=push_parser)
    parser_get = subparsers.add_parser("get", help="get ask")
    parser_get.set_defaults(func=get_parser)
    parser_push.add_argument("--cost", type=float, required=True, help="cost")
    parser_push.add_argument(
        "--key", type=str, required=True, help="key of the statement"
    )
    parser_get.add_argument("--key", type=str, help="key of the ask")
    args = parser.parse_args()
    args.func(args)
