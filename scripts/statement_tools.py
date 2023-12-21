import requests
import json
import logging
import argparse
from constants import DB_NAME, URL, MOUNT
from auth_tools import get_headers


def push(auth, file):
    if file:
        f = open(file, "r")
        data = json.load(f)
    else:
        logging.error("Error: file is required")
        return

    headers = get_headers(auth)
    url = URL + "/statement"
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Statement from {file} was pushed")
        return res


def push_parser(args):
    return push(args.auth, args.file)


def get(auth, key, output):
    headers = get_headers(auth)
    url = URL + "/statement"
    if key:
        url += key
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Statements:\n {json.dumps(res.json(), indent=4)}")
        if output:
            with open(output, "w") as f:
                json.dump(res.json(), f, indent=4)
        return res.json()


def get_parser(args):
    return get(args.auth, args.key, args.output)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parser = argparse.ArgumentParser()
    parser.add_argument("--auth", type=str, help="auth")
    subparsers = parser.add_subparsers(help="sub-command help")
    parser_push = subparsers.add_parser("push", help="push statement")
    parser_push.set_defaults(func=push_parser)
    parser_get = subparsers.add_parser("get", help="get statement")
    parser_get.set_defaults(func=get_parser)
    parser_push.add_argument("--file", type=str, required=True, help="file")
    parser_get.add_argument("--key", type=str, help="statement key")
    parser_get.add_argument("-o", "--output", type=str, help="output file")
    args = parser.parse_args()
    args.func(args=args)
