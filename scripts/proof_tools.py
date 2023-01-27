import sys
import requests
import logging
import argparse
import os.path
import inspect
import json
from constants import DB_NAME, URL, MOUNT
from auth_tools import get_headers

currentdir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
parentdir = os.path.dirname(currentdir)
sys.path.insert(0, parentdir)


def push(auth, file, bid_key=None, ask_key=None):
    proof = open(file, "r").read()
    data = {"proof": proof}
    if bid_key:
        data["bid_key"] = bid_key
    if ask_key:
        data["ask_key"] = ask_key

    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/proof"
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        return res.json()


def get(auth, bid_key=None, proof_key=None, file=None):
    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/proof/"
    if bid_key:
        url += f'?q=[{{"key" : "bid_key", "value" : "{bid_key}"}}]'
    elif proof_key:
        url += proof_key
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.reason}")
        exit(1)
    else:
        res_json = res.json()
        if file and len(res_json) > 0:
            with open(file, "w") as f:
                f.write(res_json[0].pop("proof", ""))
        logging.info(f"Proof:\t\t {json.dumps(res_json, indent=4)}")


def push_parser(args):
    push(args.auth, args.file, args.bid_key, args.ask_key)


def get_parser(args):
    get(args.auth, args.bid_key, args.proof_key, args.file)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    parser = argparse.ArgumentParser()
    parser.add_argument("--auth", type=str, help="auth")
    subparsers = parser.add_subparsers(help="sub-command help")
    parser_push = subparsers.add_parser("push", help="push proof")
    parser_push.set_defaults(func=push_parser)
    parser_get = subparsers.add_parser("get", help="get proof")
    parser_get.set_defaults(func=get_parser)
    parser_push.add_argument("-a", "--ask_key", type=str, default=None, help="ask_key")
    parser_push.add_argument("-b", "--bid_key", type=str, default=None, help="bid_key")
    parser_push.add_argument(
        "-f", "--file", type=str, required=True, help="file with proof"
    )
    parser_get.add_argument("-k", "--key", type=str, help="key of the proof")
    parser_get.add_argument("-f", "--file", type=str, help="file to write proof")
    args = parser.parse_args()
    if not args.bid_key and not args.ask_key:
        logging.error("Error: bid_key or ask_key has to be provided")
        exit(1)
    args.func(args=args)
