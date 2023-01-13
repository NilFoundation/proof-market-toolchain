import requests
import json
import logging
import argparse
from constants import DB_NAME, URL, MOUNT

def push(args):
    if args.file:
        f = open(args.file, 'r')
        data = json.load(f)
    else:
        logging.error("Error: file is required")
        return

    headers = get_headers(args)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/statement/'
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code!=200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Statement:\t {res.json()}")
        return res.json()

def get(args):
    headers = get_headers(args)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/statement/' 
    if args.key:
        url += args.key
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Statements:\n {json.dumps(res.json(), indent=4)}")
        return res.json()

def get_headers(args):
    headers = {}
    with open(args.auth, 'r') as f:
        auth = json.load(f)
        headers.update(auth)
    return headers


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('--auth', default='auth.json', type=str,
                        help='auth')
    # add subparsers for push and get
    subparsers = parser.add_subparsers(help='sub-command help')
    parser_push = subparsers.add_parser('push', help='push statement')
    parser_push.set_defaults(func=push)
    parser_get = subparsers.add_parser('get', help='get statement')
    parser_get.set_defaults(func=get)
    # add arguments for push and get
    parser_push.add_argument('--file', type=str, required=True,
                        help='file')
    parser_get.add_argument('--key', type=str,
                        help='statement key')
    args = parser.parse_args()
    args.func(args=args)
    
