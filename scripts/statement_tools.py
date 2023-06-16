import os
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
    url = URL + f"_db/{DB_NAME}/{MOUNT}/statement/"
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Statement from {file} was pushed")
        return res

def get(auth, key, output):
    headers = get_headers(auth)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/statement/"
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
    
def prepare(circuit_file, output_file, name, statement_type):
    data = {
        "name": name,
        "description": "description",
        "url": "url",
        "input_description": "input_description",
        "type": statement_type,
        "isPrivate": False,
        "definition": {
            "verification_key": "verification_key",
            "proving_key": "proving_key",
        },
        "tag": "test"
    }

    if not os.path.exists(circuit_file):
        raise FileNotFoundError(f"The circuit file '{circuit_file}' does not exist.")

    with open(circuit_file, "r") as f:
        circuit = f.read()
        data["definition"]["proving_key"] = circuit

    with open(output_file, "w") as f:
        json.dump(data, f, indent=4)

def push_parser(args):
    return push(args.auth, args.file)

def get_parser(args):
    return get(args.auth, args.key, args.output)

def prepare_parser(args):
    return prepare(args.circuit, args.output, args.name, args.type)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parser = argparse.ArgumentParser()
    parser.add_argument("--auth", type=str, help="auth")
    
    subparsers = parser.add_subparsers(help="sub-command help")
    parser_push = subparsers.add_parser("push", help="push statement")
    parser_push.set_defaults(func=push_parser)
    parser_get = subparsers.add_parser("get", help="get statement")
    parser_get.set_defaults(func=get_parser)
    parser_prepare = subparsers.add_parser("prepare", help="prepare statement")
    parser_prepare.set_defaults(func=prepare_parser)
    
    parser_push.add_argument("--file", type=str, required=True, help="file")
    parser_get.add_argument("--key", type=str, help="statement key")
    parser_get.add_argument("-o", "--output", type=str, help="output file")
    parser_prepare.add_argument(
        "-c", "--circuit", type=str, required=True, help="zkllvm compiler output"
    )
    parser_prepare.add_argument(
        "-o",
        "--output",
        metavar="output file",
        type=str,
        required=True,
        help="output file",
    )
    parser_prepare.add_argument("-n", "--name", type=str, required=True, help="name")
    parser_prepare.add_argument(
        "-t",
        "--type",
        type=str,
        required=True,
        help="statement type: placeholder-zkllvm or placeholder-vanilla",
    )
    args = parser.parse_args()
    args.func(args=args)
