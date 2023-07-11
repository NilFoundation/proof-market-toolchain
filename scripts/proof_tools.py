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


def push(auth, file, request_key=None, proposal_key=None):
    proof = open(file, "r").read()
    data = {"proof": proof}
    if request_key:
        data["request_key"] = request_key
    if proposal_key:
        data["proposal_key"] = proposal_key

    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/proof"
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        if request_key:
            logging.info(f"Proof for request {request_key} is pushed")
        else:
            logging.info(f"Proof for proposal {proposal_key} is pushed")
        return


def get(auth, request_key=None, proof_key=None, file=None):
    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/proof/"
    if request_key:
        url += f'?q=[{{"key" : "request_key", "value" : "{request_key}"}}]&full=true'
    elif proof_key:
        url += proof_key + "?full=true"
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.reason}")
        exit(1)
    else:
        res_json = res.json()
        if bid_key is not None:
            res_json = res.json()[0]
        if file and "proof" in res_json:
            with open(file, "w") as f:
                f.write(res_json.pop("proof"))
                logging.info(f"Proof is saved to {file}")
        else:
            logging.info(f"Proof:\t\t {json.dumps(res_json, indent=4)}")


def push_parser(args):
    push(args.auth, args.file, args.request_key, args.proposal_key)


def get_parser(args):
    get(args.auth, args.request_key, args.proof_key, args.file)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    parser = argparse.ArgumentParser()
    parser.add_argument("--auth", type=str, help="auth")
    subparsers = parser.add_subparsers(help="sub-command help")

    parser_push = subparsers.add_parser("push", help="push proof")
    parser_push.set_defaults(func=push_parser)
    parser_get = subparsers.add_parser("get", help="get proof")
    parser_get.set_defaults(func=get_parser)

    parser_push.add_argument("-p", "--proposal_key", type=str, default=None, help="proposal_key")
    parser_push.add_argument("-r", "--request_key", type=str, default=None, help="request_key")
    parser_push.add_argument(
        "-f", "--file", type=str, required=True, help="file with proof"
    )
    parser_get.add_argument("-k", "--proof_key", type=str, help="key of the proof")
    parser_get.add_argument("-f", "--file", type=str, help="file to write proof")
    parser_get.add_argument("-r", "--request_key", type=str, help="request_key")
    args = parser.parse_args()
    args.func(args=args)
