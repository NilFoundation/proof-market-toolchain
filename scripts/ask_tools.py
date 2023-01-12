import requests
import json
import logging
import json
import argparse
from constants import DB_NAME, URL, MOUNT

def push(data=None, args=None):
    if data is None and args:
        data = {
                "statement_key": args.key,
                "cost": args.cost,
                }
    headers = get_headers(args)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/ask/'
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Limit ask:\t {res.json()}")
        return res.json()
    
def get(args):
    headers = get_headers(args)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/ask/'
    if args.key:
        url += args.key
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return 
    else:
        logging.info(f"Ask:\n {json.dumps(res.json(), indent=4)}")
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
    parser.add_argument('--auth', type=str, default='auth.json',
                        help='auth file')
    subparsers = parser.add_subparsers(help='sub-command help')
    parser_push = subparsers.add_parser('push', help='push ask')
    parser_push.set_defaults(func=push)
    parser_get = subparsers.add_parser('get', help='get ask')
    parser_get.set_defaults(func=get)
    parser_push.add_argument('--cost', type=float, required=True,
                        help='cost')
    parser_push.add_argument('--key', type=str, required=True,
                        help='key of the statement')
    parser_get.add_argument('--key', type=str,
                        help='key of the ask')
    args = parser.parse_args()
    args.func(args=args)
    