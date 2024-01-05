import argparse
import json
import logging
import requests
import sys
from urllib.parse import urljoin

import constants
from auth_tools import get_headers


def get_prepared_input(input_file):
    f = open(input_file, "r")
    input = json.load(f)
    return input


def push(url, key, file, cost, aggregated_mode_id=None, verbose=False):
    data = {
        "statement_key": key,
        "input": get_prepared_input(file),
        "cost": cost,
    }
    if aggregated_mode_id is not None:
        data["aggregated_mode_id"] = aggregated_mode_id

    headers = get_headers(url=url)
    url = urljoin(url, "/request/")
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return None
    else:
        log_data = res.json()
        if not verbose:
            left_keys = ["_key", "status", "statement_key", "cost", "sender", "proof_key"]
            log_data = {k: v for k, v in log_data.items() if k in left_keys}
        logging.info(f"Limit request:\t {json.dumps(log_data, indent=4)}")
        return res.json()


def get(url, key=None, request_status=None, verbose=False):
    headers = get_headers(url=url)
    url = urljoin(url, "/request/")
    if request_status:
        url += f'?q=[{{"key" : "status", "value" : "{request_status}"}}]&limit=100'  # FIXME: adjust backend to either consume it from body or from query params
    elif key:
        url += str(key)
    else:
        url += "?limit=100"
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return None
    else:
        log_data = res.json()
        if not verbose and '_key' in log_data:
            left_keys = ["_key", "status", "statement_key", "cost", "sender", "proof_key"]
            log_data = {k: v for k, v in log_data.items() if k in left_keys}
        logging.info(f"Limit request:\t {json.dumps(log_data, indent=4)}")
        return res.json()


def push_parser(args):
    return push(args.url, args.statement_key, args.input, args.cost, verbose=args.verbose) is not None


def get_parser(args):
    return get(args.url, args.request_key, args.request_status, args.verbose) is not None


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument("--url", action="store", default=constants.URL, help="URL of a producer")
    parent_parser.add_argument(
        "-v", "--verbose", action="store_true", help="increase output verbosity"
    )

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(help="sub-command help")

    parser_get = subparsers.add_parser("get", help="get request", parents=[parent_parser])
    parser_get.set_defaults(func=get_parser)
    parser_get.add_argument("--request-key", type=str, help="request key")
    parser_get.add_argument("--request-status", type=str, help="request status")

    parser_push = subparsers.add_parser("push", help="push request", parents=[parent_parser])
    parser_push.set_defaults(func=push_parser)
    parser_push.add_argument("--cost", type=float, required=True, help="cost")
    parser_push.add_argument(
        "--input", type=str, required=True, help="json file with public input"
    )
    parser_push.add_argument("--statement-key", type=str, required=True, help="statement key")
    parser_push.add_argument(
        "--generation-time",
        default=30,
        type=int,
        help="required proof time generation (in mins)",
    )
    args = parser.parse_args()
    if not hasattr(args, 'func'):
        # invalid subparser
        parser.print_help()
        sys.exit(1)

    sys.exit(0 if args.func(args) else 1)
