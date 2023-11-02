import logging
import json
import argparse
import requests
from constants import DB_NAME, URL, MOUNT
from auth_tools import get_headers


def get_prepared_input(input_file):
    f = open(input_file, "r")
    input = json.load(f)
    return input

def push(auth, key, file, cost, subkey=None, verbose=False):
    data = {
        "statement_key": key,
        "input": get_prepared_input(file),
        "cost": cost,
    }
    if subkey is not None:
        data["statement_subkey"] = subkey

    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/request/"
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        log_data = res.json()
        if not verbose:
            left_keys = ["_key", "status", "statement_key", "cost", "sender", "proof_key"]
            log_data = {k: v for k, v in log_data.items() if k in left_keys}
        logging.info(f"Limit request:\t {json.dumps(log_data, indent=4)}")
        return res.json()


def get(auth, key=None, request_status=None, verbose=False):
    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/request/"
    if request_status:
        url += f'?q=[{{"key" : "status", "value" : "{request_status}"}}]&limit=100'
    elif key:
        url += key
    else:
        url += "?limit=100"
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        log_data = res.json()
        if not verbose and '_key' in log_data:
            left_keys = ["_key", "status", "statement_key", "cost", "sender", "proof_key"]
            log_data = {k: v for k, v in log_data.items() if k in left_keys}
        logging.info(f"Limit request:\t {json.dumps(log_data, indent=4)}")
        return res.json()


def push_parser(args):
    push(args.auth, args.key, args.file, args.cost, args.subkey, args.verbose)


def get_parser(args):
    get(args.auth, args.key, args.request_status, args.verbose)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parser = argparse.ArgumentParser()
    parser.add_argument("--auth", type=str, help="auth file")
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="increase output verbosity"
    )
    subparsers = parser.add_subparsers(help="sub-command help")
    parser_push = subparsers.add_parser("push", help="push request")
    parser_push.set_defaults(func=push_parser)
    parser_get = subparsers.add_parser("get", help="get request")
    parser_get.set_defaults(func=get_parser)
    parser_get.add_argument("--key", type=str, help="request key")
    parser_get.add_argument("--request_status", type=str, help="request status")
    parser_push.add_argument("--cost", type=float, required=True, help="cost")
    parser_push.add_argument(
        "--file", type=str, required=True, help="json file with public input"
    )
    parser_push.add_argument("--key", type=str, required=True, help="statement key")
    parser_push.add_argument("--subkey", type=str, help="statement key")
    parser_push.add_argument(
        "--generation_time",
        default=30,
        type=int,
        help="required proof time generation (in mins)",
    )
    args = parser.parse_args()
    args.func(args=args)
