import argparse
import json
import logging
import requests
import sys
from urllib.parse import urljoin

import constants
import request_tools
from auth_tools import get_headers


def push(url, file, request_key=None, proposal_key=None):
    proof = open(file, "r").read()
    data = {"proof": proof}
    if request_key:
        data["request_key"] = request_key
    if proposal_key:
        data["proposal_key"] = proposal_key

    headers = get_headers()
    url = urljoin(url, "/proof")
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return None

    if request_key:
        logging.info(f"Proof for request {request_key} is pushed")
    else:
        logging.info(f"Proof for proposal {proposal_key} is pushed")
    return res.json()


def get(url, request_key=None, proof_key=None, file=None):
    headers = get_headers()
    url = urljoin(url, "/proof")
    if request_key:
        proof_key = request_tools.get(key=request_key)["proof_key"]
        url += proof_key + "?full=true"
    elif proof_key:
        url += proof_key + "?full=true"
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.reason}")
        return None

    res_json = res.json()
    if file and "proof" in res_json:
        with open(file, "w") as f:
            f.write(res_json.pop("proof"))
            logging.info(f"Proof is saved to {file}")
    else:
        logging.info(f"Proof:\t\t {json.dumps(res_json, indent=4)}")

    return res_json


def push_parser(args):
    return push(args.proof, args.request_key, args.proposal_key) is not None


def get_parser(args):
    return get(args.request_key, args.proof_key, args.proof) is not None


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument(
        "--url", action="store", default=constants.URL, help="url of a producer"
    )

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(help="sub-command help")

    parser_push = subparsers.add_parser("push", help="push proof", parents=[parent_parser])
    parser_push.set_defaults(func=push_parser)
    parser_get = subparsers.add_parser("get", help="get proof", parents=[parent_parser])
    parser_get.set_defaults(func=get_parser)

    parser_push.add_argument("--proposal_key", type=str, default=None, help="proposal_key")
    parser_push.add_argument("--request_key", type=str, default=None, help="request_key")
    parser_push.add_argument(
        "-p", "--proof", type=str, required=True, help="path to read the proof"
    )
    parser_get.add_argument("--proof_key", type=str, help="key of the proof")
    parser_get.add_argument("-p", "--proof", type=str, help="path to store the proof", default="proof.bin")
    parser_get.add_argument("--request_key", type=str, help="request_key")
    args = parser.parse_args()

    args.func(args=args)
    if not hasattr(args, 'func'):
        # invalid subparser
        parser.print_help()
        sys.exit(1)

    sys.exit(0 if args.func(args) else 1)
